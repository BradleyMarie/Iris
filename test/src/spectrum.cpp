/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    render.c

Abstract:

    This file contains tests for the rendering paths.

--*/

#include <iristest.h>
using namespace IrisSpectrum;

class ConstantSpectrum final : public SpectrumBase
{
private:
    ConstantSpectrum(
        _In_ FLOAT value,
        _In_ FLOAT cutoff
        )
    : Value(value), Cutoff(cutoff)
    { }

public:
    static
    Spectrum
    Create(
        _In_ FLOAT Value,
        _In_ FLOAT Cutoff
        )
    {
        return SpectrumBase::Create(std::unique_ptr<SpectrumBase>(new ConstantSpectrum(Value, Cutoff)));
    }

    _Ret_
    virtual
    FLOAT
    Sample(
        _In_ FLOAT Wavelength
        ) const
    {
        if (Wavelength > Cutoff)
        {
            return (FLOAT) 0.0f;
        }

        return Value;
    }

private:
    FLOAT Value, Cutoff;
};

class ConstantReflector final : public ReflectorBase
{
private:
    ConstantReflector(
        _In_ FLOAT value,
        _In_ FLOAT cutoff
        )
    : Value(value), Cutoff(cutoff)
    { }

public:
    static
    Reflector
    Create(
        _In_ FLOAT Value,
        _In_ FLOAT Cutoff
        )
    {
        return ReflectorBase::Create(std::unique_ptr<ConstantReflector>(new ConstantReflector(Value, Cutoff)));
    }

    _Ret_
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const
    {
        if (Wavelength > Cutoff)
        {
            return (FLOAT) 0.0f;
        }

        return IncomingIntensity * Value;
    }

private:
    FLOAT Value, Cutoff;
};

class EmissiveReflector final : public ReflectorBase
{
private:
    EmissiveReflector(
        _In_ FLOAT value
        )
        : Value(value)
    { }

public:
    static
    Reflector
    Create(
        _In_ FLOAT Value
        )
    {
        return ReflectorBase::Create(std::unique_ptr<EmissiveReflector>(new EmissiveReflector(Value)));
    }

    _Ret_
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const
    {
        return Value;
    }

private:
    FLOAT Value;
};

TEST(NullSpectrum)
{
    Spectrum Spectr(nullptr, false);
    CHECK_EQUAL(0.0f, Spectr.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectr.Sample(2.0f));
}

TEST(ConstantValue)
{
    Spectrum Spectr = ConstantSpectrum::Create(1.0f, 1.0f);
    CHECK_EQUAL(1.0f, Spectr.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectr.Sample(2.0f));
}

TEST(AddSpectra)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);
    Spectrum Spectra1 = ConstantSpectrum::Create(2.0f, 3.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));
    CHECK_EQUAL(2.0f, Spectra1.Sample(1.0f));
    CHECK_EQUAL(2.0f, Spectra1.Sample(2.0f));
    CHECK_EQUAL(0.0f, Spectra1.Sample(4.0f));

    SpectrumReference Sum = Compositor.Add(Spectra0, Spectra1);

    CHECK_EQUAL(3.0f, Sum.Sample(1.0f));
    CHECK_EQUAL(2.0f, Sum.Sample(2.0f));
    CHECK_EQUAL(0.0f, Sum.Sample(4.0f));
}

TEST(AttenuateSpectra)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    SpectrumReference Attenuated = Compositor.Attenuate(Spectra0, 0.5);

    CHECK_EQUAL(0.5f, Attenuated.Sample(1.0f));
    CHECK_EQUAL(0.0f, Attenuated.Sample(2.0f));
}

TEST(AttenuatedAddSpectra)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);
    Spectrum Spectra1 = ConstantSpectrum::Create(2.0f, 3.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));
    CHECK_EQUAL(2.0f, Spectra1.Sample(1.0f));
    CHECK_EQUAL(2.0f, Spectra1.Sample(2.0f));
    CHECK_EQUAL(0.0f, Spectra1.Sample(4.0f));

    SpectrumReference Sum = Compositor.AttenuatedAdd(Spectra0, Spectra1, 2.0f);

    CHECK_EQUAL(5.0f, Sum.Sample(1.0f));
    CHECK_EQUAL(4.0f, Sum.Sample(2.0f));
    CHECK_EQUAL(0.0f, Sum.Sample(4.0f));
}

TEST(Reflect)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    Reflector Reflector0 = ConstantReflector::Create(0.5, 1.0f);
    SpectrumReference Reflected = Compositor.Reflect(Spectra0, Reflector0);

    CHECK_EQUAL(0.5f, Reflected.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected.Sample(2.0f));
}

TEST(AttenuatedReflect)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    Reflector Reflector0 = ConstantReflector::Create(0.5, 1.0f);
    SpectrumReference Reflected = Compositor.AttenuatedReflect(Spectra0, Reflector0, 0.5f);

    CHECK_EQUAL(0.25f, Reflected.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected.Sample(2.0f));
}

TEST(NullReflect)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0(nullptr, false);

    CHECK_EQUAL(0.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    Reflector Reflector0 = EmissiveReflector::Create(5.0f);
    SpectrumReference Reflected = Compositor.Reflect(Spectra0, Reflector0);

    CHECK_EQUAL(5.0f, Reflected.Sample(1.0f));
    CHECK_EQUAL(5.0f, Reflected.Sample(2.0f));
}

TEST(NullAttenuatedReflect)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0(nullptr, false);

    CHECK_EQUAL(0.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    Reflector Reflector0 = EmissiveReflector::Create(5.0f);
    SpectrumReference Reflected = Compositor.AttenuatedReflect(Spectra0, Reflector0, 0.5f);

    CHECK_EQUAL(2.5f, Reflected.Sample(1.0f));
    CHECK_EQUAL(2.5f, Reflected.Sample(2.0f));
}

TEST(NullReflector)
{
    Reflector Reflectror0(nullptr, false);
    CHECK_EQUAL(0.0f, Reflectror0.Reflect(1.0f, 1.0f));
    CHECK_EQUAL(0.0f, Reflectror0.Reflect(2.0f, 2.0f));
}

TEST(SimpleReflect)
{
    Reflector Reflector0 = ConstantReflector::Create(0.5, 1.0f);

    CHECK_EQUAL(0.5f, Reflector0.Reflect(1.0f, 1.0f));
    CHECK_EQUAL(0.0f, Reflector0.Reflect(2.0f, 1.0f));
}

TEST(AddReflectors)
{
    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(0.25, 1.0f);
    Reflector Reflector1 = ConstantReflector::Create(0.25, 0.5f);

    ReflectorReference SumReflector = RefCompositor.Add(Reflector0, Reflector1);

    CHECK_EQUAL(0.5f, SumReflector.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.25f, SumReflector.Reflect(0.75f, 1.0f));
    CHECK_EQUAL(0.0f, SumReflector.Reflect(2.0f, 1.0f));
}

TEST(AttenuatedAddReflectors)
{
    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(0.25, 1.0f);
    Reflector Reflector1 = ConstantReflector::Create(0.25, 0.5f);

    ReflectorReference SumReflector = RefCompositor.AttenuatedAdd(Reflector0, Reflector1, 4.0);

    CHECK_EQUAL(1.25f, SumReflector.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.25f, SumReflector.Reflect(0.75f, 1.0f));
    CHECK_EQUAL(0.0f, SumReflector.Reflect(2.0f, 1.0f));
}

TEST(AttenuateReflectors)
{
    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(0.25, 1.0f);
    ReflectorReference Reflector1 = RefCompositor.Attenuate(Reflector0, 2.0);

    CHECK_EQUAL(0.5f, Reflector1.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.0f, Reflector1.Reflect(2.0f, 1.0f));
}