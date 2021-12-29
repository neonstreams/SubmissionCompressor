#include "JuceHeader.h"

template <typename SampleType>
class Compressor
{
public:
    //==============================================================================
    /** Constructor. */
    Compressor();

    //==============================================================================
    /** Sets the threshold in dB of the compressor.*/
    void setThreshold(SampleType newThreshold);

    /** Sets the ratio of the compressor (must be higher or equal to 1).*/
    void setRatio(SampleType newRatio);

    /** Sets the attack time in milliseconds of the compressor.*/
    void setAttack(SampleType newAttack);

    /** Sets the release time in milliseconds of the compressor.*/
    void setRelease(SampleType newRelease);

    /** Sets the knee of the compressor.*/
    void setKnee(SampleType newKnee);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context, bool dualMono) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        if (context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        if (dualMono)
        {
            // SUM LEFT AND RIGHT
            auto* inputSamplesLeft = inputBlock.getChannelPointer(0);
            auto* inputSamplesRight = inputBlock.getChannelPointer(1);
            //auto monoSummed = inputSamplesLeft + inputSamplesRight;

            // SPEAKERS (CHANNELS L AND R)
            auto* outputSamplesLeft = outputBlock.getChannelPointer(0);
            auto* outputSamplesRight = outputBlock.getChannelPointer(1);

            for (size_t i = 0; i < numSamples; ++i)
            {
                outputSamplesLeft[i] = processSample(0, inputSamplesLeft[i]);
                outputSamplesRight[i] = outputSamplesLeft[i];
            }
        }
        else
        {
            for (size_t channel = 0; channel < numChannels; ++channel)
            {
                auto* inputSamples = inputBlock.getChannelPointer(channel);
                auto* outputSamples = outputBlock.getChannelPointer(channel);

                for (size_t i = 0; i < numSamples; ++i)
                    outputSamples[i] = processSample(static_cast<int>(channel), inputSamples[i]);
            }
        }
    }

    /** Performs the processing operation on a single sample at a time. */
    SampleType processSample(int channel, SampleType inputValue);

private:
    //==============================================================================
    void update();

    //==============================================================================
    SampleType threshold, thresholdInverse, ratioInverse;
    juce::dsp::BallisticsFilter<SampleType> envelopeFilter;

    double sampleRate = 44100.0f;
    SampleType thresholddB = 0.0f, ratio = 1.0f, attackTime = 1.0f, releaseTime = 100.0f, knee = 1.0f;
};