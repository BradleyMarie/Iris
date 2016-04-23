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

TEST(SpectrumCompositor)
{
    SpectrumCompositor SpecCompositor0 = SpectrumCompositor::Create();
    ReflectorCompositor RefCompositor0 = ReflectorCompositor::Create();

    Reflector Reflector0 = EmissiveReflector::Create(1.0f);
    Reflector Reflector1 = EmissiveReflector::Create(2.0f);
    ReflectorReference Reflector2 = RefCompositor0.Attenuate(Reflector0, 0.0f);
    ReflectorReference Reflector3 = RefCompositor0.Attenuate(Reflector0, 1.0f);
    ReflectorReference Reflector4 = RefCompositor0.Attenuate(Reflector0, 2.0f);

    std::vector<ReflectorReference> Reflectors = { ReflectorReference(Reflector0.AsPCREFLECTOR()),
                                                   ReflectorReference(Reflector1.AsPCREFLECTOR()),
                                                   Reflector2,
                                                   Reflector3, 
                                                   Reflector4,
                                                   ReflectorReference(nullptr) };

    std::vector<FLOAT> ReflectorAnswers = { 1.0f, 2.0f, 0.0f, 1.0f, 2.0f, 0.0f };

    Spectrum Spectrum0 = ConstantSpectrum::Create(1.0f, 1.5f);
    Spectrum Spectrum1 = ConstantSpectrum::Create(2.0f, 1.5f);
    Spectrum Spectrum2 = ConstantSpectrum::Create(4.0f, 1.5f);
    Spectrum Spectrum3 = ConstantSpectrum::Create(8.0f, 1.5f);

    std::vector<SpectrumReference> Spectra = { SpectrumReference(nullptr),
                                               SpectrumReference(Spectrum0.AsPCSPECTRUM()),
                                               SpectrumReference(Spectrum1.AsPCSPECTRUM()),
                                               SpectrumReference(Spectrum2.AsPCSPECTRUM()),
                                               SpectrumReference(Spectrum3.AsPCSPECTRUM()) };

    std::vector<FLOAT> AnswersWavelengthOne = { 0.0f, 1.0f, 2.0f, 4.0f, 8.0f };
    std::vector<FLOAT> AnswersWavelengthTwo = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    for (size_t i = 0; i < Reflectors.size(); i++)
    {
        Spectra.push_back(SpecCompositor0.Reflect(SpectrumReference(nullptr), Reflectors[i]));
        AnswersWavelengthOne.push_back(ReflectorAnswers[i]);
        AnswersWavelengthTwo.push_back(ReflectorAnswers[i]);
    }

    for (size_t i = 0; i < Reflectors.size(); i++)
    {
        Spectra.push_back(SpecCompositor0.AttenuatedReflect(SpectrumReference(nullptr), Reflectors[i], 0.0f));
        Spectra.push_back(SpecCompositor0.AttenuatedReflect(SpectrumReference(nullptr), Reflectors[i], 0.5f));
        Spectra.push_back(SpecCompositor0.AttenuatedReflect(SpectrumReference(nullptr), Reflectors[i], 1.0f));
        Spectra.push_back(SpecCompositor0.AttenuatedReflect(SpectrumReference(nullptr), Reflectors[i], 2.0f));
        AnswersWavelengthOne.push_back(ReflectorAnswers[i] * 0.0f);
        AnswersWavelengthOne.push_back(ReflectorAnswers[i] * 0.5f);
        AnswersWavelengthOne.push_back(ReflectorAnswers[i] * 1.0f);
        AnswersWavelengthOne.push_back(ReflectorAnswers[i] * 2.0f);
        AnswersWavelengthTwo.push_back(ReflectorAnswers[i] * 0.0f);
        AnswersWavelengthTwo.push_back(ReflectorAnswers[i] * 0.5f);
        AnswersWavelengthTwo.push_back(ReflectorAnswers[i] * 1.0f);
        AnswersWavelengthTwo.push_back(ReflectorAnswers[i] * 2.0f);
    }

    size_t StopSize = Spectra.size();

    for (size_t i = 0; i < StopSize; i++)
    {
        Spectra.push_back(SpecCompositor0.Attenuate(Spectra[i], 0.0f));
        Spectra.push_back(SpecCompositor0.Attenuate(Spectra[i], 0.5f));
        Spectra.push_back(SpecCompositor0.Attenuate(Spectra[i], 1.0f));
        Spectra.push_back(SpecCompositor0.Attenuate(Spectra[i], 2.0f));
        AnswersWavelengthOne.push_back(AnswersWavelengthOne[i] * 0.0f);
        AnswersWavelengthOne.push_back(AnswersWavelengthOne[i] * 0.5f);
        AnswersWavelengthOne.push_back(AnswersWavelengthOne[i] * 1.0f);
        AnswersWavelengthOne.push_back(AnswersWavelengthOne[i] * 2.0f);
        AnswersWavelengthTwo.push_back(AnswersWavelengthTwo[i] * 0.0f);
        AnswersWavelengthTwo.push_back(AnswersWavelengthTwo[i] * 0.5f);
        AnswersWavelengthTwo.push_back(AnswersWavelengthTwo[i] * 1.0f);
        AnswersWavelengthTwo.push_back(AnswersWavelengthTwo[i] * 2.0f);
    }

    StopSize = Spectra.size();

    for (size_t i = 0; i < StopSize; i++)
    {
        for (size_t j = 0; j < StopSize; j++)
        {
            Spectra.push_back(SpecCompositor0.Add(Spectra[i], Spectra[j]));
            AnswersWavelengthOne.push_back(AnswersWavelengthOne[i] + AnswersWavelengthOne[j]);
            AnswersWavelengthTwo.push_back(AnswersWavelengthTwo[i] + AnswersWavelengthTwo[j]);
        }
    }

    for (size_t i = 0; i < Spectra.size(); i++)
    {
        CHECK_EQUAL(AnswersWavelengthOne[i], Spectra[i].Sample(1.0f));
        CHECK_EQUAL(AnswersWavelengthTwo[i], Spectra[i].Sample(2.0f));
    }

    SpectrumCompositor SpecCompositor1 = SpectrumCompositor::Create();

    for (size_t i = 0; i < StopSize; i++)
    {
        for (size_t j = 0; j < StopSize; j++)
        {
            SpectrumReference TempSpectrum = SpecCompositor1.Add(Spectra[i], Spectra[j]);
            CHECK_EQUAL(AnswersWavelengthOne[i] + AnswersWavelengthOne[j], TempSpectrum.Sample(1.0f));
            CHECK_EQUAL(AnswersWavelengthTwo[i] + AnswersWavelengthTwo[j], TempSpectrum.Sample(2.0f));
            SpecCompositor1.Clear();
        }
    } 
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

TEST(TestReflectorCompositor)
{
    ReflectorCompositor RefCompositor0 = ReflectorCompositor::Create();

    Reflector Reflector0 = EmissiveReflector::Create(1.0f);
    Reflector Reflector1 = EmissiveReflector::Create(2.0f);
    Reflector Reflector2 = EmissiveReflector::Create(4.0f);
    Reflector Reflector3 = EmissiveReflector::Create(8.0f);

    std::vector<ReflectorReference> References = { ReflectorReference(nullptr),
                                                   RefCompositor0.Attenuate(Reflector0, 0.5),
                                                   ReflectorReference(Reflector0.AsPCREFLECTOR()),
                                                   RefCompositor0.Attenuate(Reflector0, 2.0),
                                                   RefCompositor0.Attenuate(Reflector1, 0.5),
                                                   ReflectorReference(Reflector1.AsPCREFLECTOR()),
                                                   RefCompositor0.Attenuate(Reflector1, 2.0),
                                                   RefCompositor0.Attenuate(Reflector2, 0.5),
                                                   ReflectorReference(Reflector2.AsPCREFLECTOR()),
                                                   RefCompositor0.Attenuate(Reflector2, 2.0),
                                                   RefCompositor0.Attenuate(Reflector3, 0.5),
                                                   ReflectorReference(Reflector3.AsPCREFLECTOR()),
                                                   RefCompositor0.Attenuate(Reflector3, 2.0) };

    std::vector<FLOAT> Answers = { 0.0f, 0.5f, 1.0f, 2.0f, 1.0f, 2.0f, 4.0f, 2.0f, 4.0f, 8.0f, 4.0f, 8.0f, 16.0f };

    for (size_t i = 0; i < References.size(); i++)
    {
        CHECK_EQUAL(Answers[i], References[i].Reflect(1.0f, 1.0f));
    }

    //
    // Test Add Reflector Routines
    //

    size_t StopSize = References.size();

    for (size_t i = 0; i < StopSize; i++)
    {
        for (size_t j = 0; j < StopSize; j++)
        {
            References.push_back(RefCompositor0.Add(References[i], References[j]));
            Answers.push_back(Answers[i] + Answers[j]);
        }
    }

    //
    // Do initial sums twice
    //

    for (size_t i = 0; i < StopSize; i++)
    {
        for (size_t j = 0; j < StopSize; j++)
        {
            References.push_back(RefCompositor0.Add(References[i], References[j]));
            Answers.push_back(Answers[i] + Answers[j]);
        }
    }

    StopSize = References.size();

    for (size_t i = 0; i < StopSize; i++)
    {
        for (size_t j = 0; j < StopSize; j++)
        {
            References.push_back(RefCompositor0.Add(References[i], References[j]));
            Answers.push_back(Answers[i] + Answers[j]);
        }
    }

    //
    // Test Attenuate Reflector Routines
    //

    StopSize = References.size();

    for (size_t i = 0; i < StopSize; i++)
    {
        References.push_back(RefCompositor0.Attenuate(References[i], 0.0f));
        Answers.push_back(Answers[i] * 0.0f);

        References.push_back(RefCompositor0.Attenuate(References[i], 0.5f));
        Answers.push_back(Answers[i] * 0.5f);

        References.push_back(RefCompositor0.Attenuate(References[i], 1.0f));
        Answers.push_back(Answers[i] * 1.0f);

        References.push_back(RefCompositor0.Attenuate(References[i], 2.0f));
        Answers.push_back(Answers[i] * 2.0f);
    }

    //
    // Validate Answers
    //

    for (size_t i = 0; i < References.size(); i++)
    {
        CHECK_EQUAL(Answers[i], References[i].Reflect(1.0f, 1.0f));
    }

    //
    // Compute Sums Again
    //

    ReflectorCompositor RefCompositor1 = ReflectorCompositor::Create();

    for (size_t i = 0; i < References.size(); i++)
    {
        CHECK_EQUAL(Answers[i] * 0.0f, RefCompositor1.Attenuate(References[i], 0.0f).Reflect(1.0f, 1.0f));
        CHECK_EQUAL(Answers[i] * 0.5f, RefCompositor1.Attenuate(References[i], 0.5f).Reflect(1.0f, 1.0f));
        CHECK_EQUAL(Answers[i] * 1.0f, RefCompositor1.Attenuate(References[i], 1.0f).Reflect(1.0f, 1.0f));
        CHECK_EQUAL(Answers[i] * 2.0f, RefCompositor1.Attenuate(References[i], 2.0f).Reflect(1.0f, 1.0f));
        RefCompositor1.Clear();
    }
}