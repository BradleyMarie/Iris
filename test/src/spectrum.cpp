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

    SpectrumReference Sum0 = Compositor.Add(Spectra0, Spectra1);

    CHECK_EQUAL(3.0f, Sum0.Sample(1.0f));
    CHECK_EQUAL(2.0f, Sum0.Sample(2.0f));
    CHECK_EQUAL(0.0f, Sum0.Sample(4.0f));

    SpectrumReference Sum1 = Compositor.Add(Spectra0, Spectra0);

    CHECK_EQUAL(2.0f, Sum1.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum1.Sample(2.0f));
    CHECK_EQUAL(0.0f, Sum1.Sample(4.0f));

    SpectrumReference Sum2 = Compositor.Add(Sum1, Sum1);

    CHECK_EQUAL(4.0f, Sum2.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum2.Sample(2.0f));
    CHECK_EQUAL(0.0f, Sum2.Sample(4.0f));

    SpectrumReference Sum3 = Compositor.Add(Sum2, Sum0);

    CHECK_EQUAL(7.0f, Sum3.Sample(1.0f));
    CHECK_EQUAL(2.0f, Sum3.Sample(2.0f));
    CHECK_EQUAL(0.0f, Sum3.Sample(4.0f));

    Spectrum Spectra2 = ConstantSpectrum::Create(1.0f, 1.0f);
    Spectrum Spectra3 = ConstantSpectrum::Create(2.0f, 1.0f);
    Spectrum Spectra4 = ConstantSpectrum::Create(3.0f, 1.0f);
    Spectrum Spectra5 = ConstantSpectrum::Create(4.0f, 1.0f);

    SpectrumReference Sum4 = Compositor.Add(Compositor.Add(Spectra2, Spectra3),
                                            Compositor.Add(Spectra2, Spectra3));

    CHECK_EQUAL(6.0f, Sum4.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum4.Sample(2.0f));

    SpectrumReference Sum5 = Compositor.Add(Compositor.Add(Spectra3, Spectra2),
                                            Compositor.Add(Spectra2, Spectra3));

    CHECK_EQUAL(6.0f, Sum5.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum5.Sample(2.0f));

    SpectrumReference Sum6 = Compositor.Add(Compositor.Add(Spectra2, Spectra4),
                                            Compositor.Add(Spectra2, Spectra3));

    CHECK_EQUAL(7.0f, Sum6.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum6.Sample(2.0f));

    SpectrumReference Sum7 = Compositor.Add(Compositor.Add(Spectra4, Spectra2),
                                            Compositor.Add(Spectra2, Spectra3));

    CHECK_EQUAL(7.0f, Sum7.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum7.Sample(2.0f));

    SpectrumReference Sum8 = Compositor.Add(Compositor.Add(Spectra4, Spectra2),
                                            Compositor.Add(Spectra3, Spectra2));

    CHECK_EQUAL(7.0f, Sum8.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum8.Sample(2.0f));

    SpectrumReference Sum9 = Compositor.Add(Compositor.Add(Spectra2, Spectra4),
                                            Compositor.Add(Spectra3, Spectra2));

    CHECK_EQUAL(7.0f, Sum9.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum9.Sample(2.0f));
}

TEST(AttenuateSpectra)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);
    Spectrum Spectra1 = ConstantSpectrum::Create(1.0f, 1.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    SpectrumReference Attenuated0 = Compositor.Attenuate(Spectra0, 0.5);

    CHECK_EQUAL(0.5f, Attenuated0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Attenuated0.Sample(2.0f));

    SpectrumReference Attenuated1 = Compositor.Attenuate(Attenuated0, 0.5);

    CHECK_EQUAL(0.25f, Attenuated1.Sample(1.0f));
    CHECK_EQUAL(0.0f, Attenuated1.Sample(2.0f));

    SpectrumReference Sum0 = Compositor.Add(Attenuated0, Attenuated1);

    CHECK_EQUAL(0.75f, Sum0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum0.Sample(2.0f));

    SpectrumReference Attenuated2 = Compositor.Attenuate(Spectra1, 0.5);
    SpectrumReference Sum2 = Compositor.Add(Attenuated0, Attenuated2);

    CHECK_EQUAL(1.0f, Sum2.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum2.Sample(2.0f));
}

TEST(Reflect)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    Reflector Reflector0 = ConstantReflector::Create(0.5, 1.0f);
    SpectrumReference Reflected0 = Compositor.Reflect(Spectra0, Reflector0);

    CHECK_EQUAL(0.5f, Reflected0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected0.Sample(2.0f));

    SpectrumReference Reflected1 = Compositor.Reflect(Spectra0, Reflector0);

    SpectrumReference Sum0 = Compositor.Add(Reflected0, Reflected1);

    CHECK_EQUAL(1.0f, Sum0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum0.Sample(2.0f));
}

TEST(AttenuatedReflect)
{
    SpectrumCompositor Compositor = SpectrumCompositor::Create();

    Spectrum Spectra0 = ConstantSpectrum::Create(1.0f, 1.0f);
    Spectrum Spectra1 = ConstantSpectrum::Create(1.0f, 1.0f);

    CHECK_EQUAL(1.0f, Spectra0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Spectra0.Sample(2.0f));

    Reflector Reflector0 = ConstantReflector::Create(0.5, 1.0f);
    SpectrumReference Reflected0 = Compositor.AttenuatedReflect(Spectra0, Reflector0, 0.5f);

    CHECK_EQUAL(0.25f, Reflected0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected0.Sample(2.0f));

    SpectrumReference Reflected1 = Compositor.AttenuatedReflect(Spectra0, Reflector0, 2.0f);

    SpectrumReference Sum0 = Compositor.Add(Reflected0, Reflected1);

    CHECK_EQUAL(1.25f, Sum0.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum0.Sample(2.0f));

    Reflector Reflector1 = ConstantReflector::Create(0.5, 1.0f);
    SpectrumReference Reflected2 = Compositor.AttenuatedReflect(Spectra0, Reflector1, 0.5f);

    SpectrumReference Sum1 = Compositor.Add(Reflected0, Reflected2);

    CHECK_EQUAL(0.5f, Sum1.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum1.Sample(2.0f));

    SpectrumReference Attenuated0 = Compositor.Attenuate(Spectra1, 0.5f);
    SpectrumReference Sum2 = Compositor.Add(Reflected0, Attenuated0);

    CHECK_EQUAL(0.75f, Sum2.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum2.Sample(2.0f));

    SpectrumReference Sum3 = Compositor.Add(Attenuated0, Reflected0);

    CHECK_EQUAL(0.75f, Sum3.Sample(1.0f));
    CHECK_EQUAL(0.0f, Sum3.Sample(2.0f));

    SpectrumReference Reflected3 = Compositor.AttenuatedReflect(Spectra0, Reflector1, 1.0f);
    CHECK_EQUAL(0.5f, Reflected3.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected3.Sample(2.0f));

    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();
    ReflectorReference Reflector2 = RefCompositor.Attenuate(Reflector1, 0.5f);
    SpectrumReference Reflected4 = Compositor.AttenuatedReflect(Spectra0, Reflector2, 1.0f);

    CHECK_EQUAL(0.25f, Reflected4.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected4.Sample(2.0f));

    SpectrumReference Reflected5 = Compositor.AttenuatedReflect(Spectra0, Reflector2, 4.0f);

    CHECK_EQUAL(1.0f, Reflected5.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected5.Sample(2.0f));

    SpectrumReference Reflected6 = Compositor.Attenuate(Reflected5, 4.0f);

    CHECK_EQUAL(4.0f, Reflected6.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected6.Sample(2.0f));
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
    SpectrumReference Reflected0 = Compositor.AttenuatedReflect(Spectra0, Reflector0, 0.5f);

    CHECK_EQUAL(2.5f, Reflected0.Sample(1.0f));
    CHECK_EQUAL(2.5f, Reflected0.Sample(2.0f));

    SpectrumReference Reflected1 = Compositor.AttenuatedReflect(Spectra0, Reflector0, 0.0);

    CHECK_EQUAL(0.0f, Reflected1.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected1.Sample(2.0f));

    ReflectorReference Reflector1(nullptr);
    SpectrumReference Reflected2 = Compositor.AttenuatedReflect(Spectra0, Reflector1, 100.0);

    CHECK_EQUAL(0.0f, Reflected2.Sample(1.0f));
    CHECK_EQUAL(0.0f, Reflected2.Sample(2.0f));
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
    ReflectorCompositor Compositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(0.25, 1.0f);
    Reflector Reflector1 = ConstantReflector::Create(0.25, 0.5f);

    ReflectorReference SumReflector = Compositor.Add(Reflector0, Reflector1);

    CHECK_EQUAL(0.5f, SumReflector.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.25f, SumReflector.Reflect(0.75f, 1.0f));
    CHECK_EQUAL(0.0f, SumReflector.Reflect(2.0f, 1.0f));

    Reflector Reflector2 = ConstantReflector::Create(1.0f, 1.0f);
    Reflector Reflector3 = ConstantReflector::Create(2.0f, 1.0f);
    Reflector Reflector4 = ConstantReflector::Create(3.0f, 1.0f);
    Reflector Reflector5 = ConstantReflector::Create(4.0f, 1.0f);

    ReflectorReference Sum4 = Compositor.Add(Compositor.Add(Reflector2, Reflector3),
                                             Compositor.Add(Reflector2, Reflector3));

    CHECK_EQUAL(6.0f, Sum4.Reflect(0.5f, 1.0f));
    CHECK_EQUAL(0.0f, Sum4.Reflect(2.0f, 1.0f));

    ReflectorReference Sum5 = Compositor.Add(Compositor.Add(Reflector3, Reflector2),
                                             Compositor.Add(Reflector2, Reflector3));

    CHECK_EQUAL(6.0f, Sum5.Reflect(0.5f, 1.0f));
    CHECK_EQUAL(0.0f, Sum5.Reflect(2.0f, 1.0f));

    ReflectorReference Sum6 = Compositor.Add(Compositor.Add(Reflector2, Reflector4),
                                             Compositor.Add(Reflector2, Reflector3));

    CHECK_EQUAL(7.0f, Sum6.Reflect(0.5f, 1.0f));
    CHECK_EQUAL(0.0f, Sum6.Reflect(2.0f, 1.0f));

    ReflectorReference Sum7 = Compositor.Add(Compositor.Add(Reflector4, Reflector2),
                                             Compositor.Add(Reflector2, Reflector3));

    CHECK_EQUAL(7.0f, Sum7.Reflect(0.5f, 1.0f));
    CHECK_EQUAL(0.0f, Sum7.Reflect(2.0f, 1.0f));

    ReflectorReference Sum8 = Compositor.Add(Compositor.Add(Reflector4, Reflector2),
                                             Compositor.Add(Reflector3, Reflector2));

    CHECK_EQUAL(7.0f, Sum8.Reflect(0.5f, 1.0f));
    CHECK_EQUAL(0.0f, Sum8.Reflect(2.0f, 1.0f));

    ReflectorReference Sum9 = Compositor.Add(Compositor.Add(Reflector2, Reflector4),
                                             Compositor.Add(Reflector3, Reflector2));

    CHECK_EQUAL(7.0f, Sum9.Reflect(0.5f, 1.0f));
    CHECK_EQUAL(0.0f, Sum9.Reflect(2.0f, 1.0f));
}

TEST(AttenuateReflectors)
{
    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(0.25, 1.0f);
    ReflectorReference Reflector1 = RefCompositor.Attenuate(Reflector0, 2.0);

    CHECK_EQUAL(0.5f, Reflector1.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.0f, Reflector1.Reflect(2.0f, 1.0f));

    ReflectorReference Reflector2 = RefCompositor.Attenuate(Reflector1, 0.0);

    CHECK_EQUAL(0.0f, Reflector2.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.0f, Reflector2.Reflect(2.0f, 1.0f));

    ReflectorReference Reflector3(nullptr);
    ReflectorReference Reflector4 = RefCompositor.Attenuate(Reflector3, 0.0);

    CHECK_EQUAL(0.0f, Reflector4.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.0f, Reflector4.Reflect(2.0f, 1.0f));
}

TEST(AddSameReflector)
{
    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(2.0, 1.0f);

    ReflectorReference SumReflector = RefCompositor.Add(Reflector0, Reflector0);

    CHECK_EQUAL(4.0f, SumReflector.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(4.0f, SumReflector.Reflect(0.75f, 1.0f));
    CHECK_EQUAL(0.0f, SumReflector.Reflect(2.0f, 1.0f));
}

TEST(AttenuateSameReflector)
{
    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(1.0, 1.0f);
    ReflectorReference Reflector1 = RefCompositor.Attenuate(Reflector0, 2.0);
    ReflectorReference Reflector2 = RefCompositor.Attenuate(Reflector1, 2.0);

    CHECK_EQUAL(4.0f, Reflector2.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.0f, Reflector2.Reflect(2.0f, 1.0f));

    ReflectorReference Reflector3 = RefCompositor.Attenuate(Reflector0, 0.5);
    ReflectorReference Reflector4 = RefCompositor.Attenuate(Reflector3, 2.0);

    CHECK_EQUAL(1.0f, Reflector4.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(0.0f, Reflector4.Reflect(2.0f, 1.0f));
}

TEST(AddAttenuatedReflectors)
{
    ReflectorCompositor RefCompositor = ReflectorCompositor::Create();

    Reflector Reflector0 = ConstantReflector::Create(2.0, 1.0f);
    Reflector Reflector1 = ConstantReflector::Create(1.0, 1.0f);

    ReflectorReference Reflector2 = RefCompositor.Attenuate(Reflector0, 0.5);
    ReflectorReference Reflector3 = RefCompositor.Attenuate(Reflector1, 0.5);
    ReflectorReference SumReflector0 = RefCompositor.Add(Reflector2, Reflector3);

    ReflectorReference Reflector4 = RefCompositor.Attenuate(Reflector0, 0.5);
    ReflectorReference Reflector5 = RefCompositor.Attenuate(Reflector0, 1.5);
    ReflectorReference SumReflector1 = RefCompositor.Add(Reflector4, Reflector5);

    ReflectorReference SumReflector3 = RefCompositor.Add(SumReflector0, SumReflector1);

    CHECK_EQUAL(5.5f, SumReflector3.Reflect(0.25f, 1.0f));
    CHECK_EQUAL(5.5f, SumReflector3.Reflect(0.75f, 1.0f));
    CHECK_EQUAL(0.0f, SumReflector3.Reflect(2.0f, 1.0f));
}

TEST(AddLotsOfReflectors)
{
    ReflectorCompositor Compositor = ReflectorCompositor::Create();

    std::vector<Reflector> BaseReflectors = { EmissiveReflector::Create(1.0f),
                                              EmissiveReflector::Create(2.0f),
                                              EmissiveReflector::Create(4.0f),
                                              EmissiveReflector::Create(8.0f) };

    std::vector<ReflectorReference> References = { Compositor.Attenuate(BaseReflectors[0], 0.5),
                                                   Compositor.Attenuate(BaseReflectors[0], 1.0),
                                                   Compositor.Attenuate(BaseReflectors[0], 2.0),
                                                   Compositor.Attenuate(BaseReflectors[1], 0.5),
                                                   Compositor.Attenuate(BaseReflectors[1], 1.0),
                                                   Compositor.Attenuate(BaseReflectors[1], 2.0),
                                                   Compositor.Attenuate(BaseReflectors[2], 0.5),
                                                   Compositor.Attenuate(BaseReflectors[2], 1.0),
                                                   Compositor.Attenuate(BaseReflectors[2], 2.0),
                                                   Compositor.Attenuate(BaseReflectors[3], 0.5),
                                                   Compositor.Attenuate(BaseReflectors[3], 1.0),
                                                   Compositor.Attenuate(BaseReflectors[3], 2.0) };

    std::vector<FLOAT> Answers = { 0.5f, 1.0f, 2.0f, 1.0f, 2.0f, 4.0f, 2.0f, 4.0f, 8.0f, 4.0f, 8.0f, 16.0f };

    for (size_t i = 0; i < References.size(); i++)
    {
        CHECK_EQUAL(Answers[i], References[i].Reflect(1.0f, 1.0f));
    }

    size_t StopSize = References.size();

    for (size_t i = 0; i < StopSize; i++)
    {
        for (size_t j = 0; j < StopSize; j++)
        {
            References.push_back(Compositor.Add(References[i], References[j]));
            Answers.push_back(Answers[i] + Answers[j]);
        }
    }

    StopSize = References.size();

    for (size_t i = 0; i < StopSize; i++)
    {
        for (size_t j = 0; j < StopSize; j++)
        {
            References.push_back(Compositor.Add(References[i], References[j]));
            Answers.push_back(Answers[i] + Answers[j]);
        }
    }

    for (size_t i = 0; i < References.size(); i++)
    {
        CHECK_EQUAL(Answers[i], References[i].Reflect(1.0f, 1.0f));
    }
}