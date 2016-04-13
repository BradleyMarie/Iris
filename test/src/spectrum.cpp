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

TEST(ConstantValue)
{
    Spectrum Spectr = ConstantSpectrum::Create((FLOAT) 1.0f, (FLOAT) 1.0f);
    CHECK_EQUAL(1.0f, Spectr.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectr.Sample(2.0f));
}

TEST(AddSpectra)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create((FLOAT) 1.0f, (FLOAT) 1.0f);
    Spectrum Spectra1 = ConstantSpectrum::Create((FLOAT) 2.0f, (FLOAT) 3.0f);

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