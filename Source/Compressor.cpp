#include "Compressor.h"

//==============================================================================
template <typename SampleType>
Compressor<SampleType>::Compressor()
{
    update();
}

//==============================================================================
template <typename SampleType>
void Compressor<SampleType>::setThreshold(SampleType newThreshold)
{
    thresholddB = newThreshold;
    update();
}

template <typename SampleType>
void Compressor<SampleType>::setRatio(SampleType newRatio)
{
    jassert(newRatio >= static_cast<SampleType> (1.0));

    ratio = newRatio;
    update();
}

template <typename SampleType>
void Compressor<SampleType>::setAttack(SampleType newAttack)
{
    attackTime = newAttack;
    update();
}

template <typename SampleType>
void Compressor<SampleType>::setRelease(SampleType newRelease)
{
    releaseTime = newRelease;
    update();
}

template <typename SampleType>
void Compressor<SampleType>::setKnee(SampleType newKnee)
{
    kneeInDbUnits = newKnee;
    update();
}

//==============================================================================
template <typename SampleType>
void Compressor<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    envelopeFilter.prepare(spec);

    update();
    reset();
}

template <typename SampleType>
void Compressor<SampleType>::reset()
{
    envelopeFilter.reset();
}

//==============================================================================
template <typename SampleType>
SampleType Compressor<SampleType>::processSample(int channel, SampleType inputValue)
{
    //inputSampleAmplitudeinDB
    auto inputValueInDb = juce::Decibels::gainToDecibels(inputValue, static_cast<SampleType> (-200.0));

    // Ballistics filter with peak rectifier
    auto env = envelopeFilter.processSample(channel, inputValue);

    //VCA
    auto gain = (env < threshold) ? static_cast<SampleType> (1.0) : std::pow(env * thresholdInverse, ratioInverse - static_cast<SampleType> (1.0));

    return gain * inputValue;
}

template <typename SampleType>
void Compressor<SampleType>::update()
{
    threshold = juce::Decibels::decibelsToGain(thresholddB, static_cast<SampleType> (-200.0));
    thresholdInverse = static_cast<SampleType> (1.0) / threshold;
    ratioInverse = static_cast<SampleType> (1.0) / ratio;
    slope = static_cast<SampleType>(1.0) / (ratio - (1.0));

    lowerKneeBoundDbUnits = thresholddB - (kneeInDbUnits / 2);
    upperKneeBoundDbUnits = thresholddB + (kneeInDbUnits / 2);
    kneeInGainUnits = juce::Decibels::decibelsToGain(kneeInDbUnits, static_cast<SampleType> (-200.0));

    envelopeFilter.setAttackTime(attackTime);
    envelopeFilter.setReleaseTime(releaseTime);
}

//==============================================================================
template class Compressor<float>;
template class Compressor<double>;