/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include <opencv2/opencv.hpp>
#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace juce;


//==============================================================================
GuitarPluginAudioProcessor::GuitarPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    //distortion state
    state = new AudioProcessorValueTreeState(*this, NULL);
    state->createAndAddParameter("distortion_driveAttachment", "distortion_DriveName", "distortion_DriveLabel", NormalisableRange<float>(0.f, 1.f), 1.f, NULL, NULL);
    state->createAndAddParameter("distortion_rangeAttachment", "distortion_RangeName", "distortion_RangeLabel", NormalisableRange<float>(0.f, 3000.f), 1.f, NULL, NULL);
    state->createAndAddParameter("distortion_blendAttachment", "distortion_BlendName", "distortion_BlendLabel", NormalisableRange<float>(0.f, 1.f), 1.f, NULL, NULL);
    state->createAndAddParameter("distortion_volumeAttachment", "distortion_VolumeName", "distortion_VolumeLabel", NormalisableRange<float>(0.f, 1.f), 1.f, NULL, NULL);

    state->createAndAddParameter("reverb_roomSizeAttachment", "reverb_roomSizeName", "reverb_roomSizeLabel", NormalisableRange<float>(0.f, 1.f), 1.f, NULL, NULL);
    state->createAndAddParameter("reverb_wetDryAttachment", "reverb_WetDryName", "reverb_WetDryLabel", NormalisableRange<float>(0.f, 1.f), 1.f, NULL, NULL);
    state->createAndAddParameter("reverb_dampingAttachment", "reverb_DampingName", "reverb_DampingLabel", NormalisableRange<float>(0.f, 1.f), 1.f, NULL, NULL);
    state->createAndAddParameter("reverb_widthAttachment", "reverb_WidthName", "reverb_WidthLabel", NormalisableRange<float>(0.f, 1.f), 1.f, NULL, NULL);

    state->state = ValueTree("distortion_DriveValueTreeID");
    state->state = ValueTree("distortion_RangeValueTreeID");
    state->state = ValueTree("distortion_BlendValueTreeID");
    state->state = ValueTree("distortion_VolumeValueTreeID");

    //reverb state

    state->state = ValueTree("reverb_roomSizeValueTreeID");
    state->state = ValueTree("reverb_WetDryValueTreeID");
    state->state = ValueTree("reverb_DampingValueTreeID");
    state->state = ValueTree("reverb_WidthValueTreeID");

}

GuitarPluginAudioProcessor::~GuitarPluginAudioProcessor()
{
}

//==============================================================================
const String GuitarPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GuitarPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GuitarPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double GuitarPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarPluginAudioProcessor::setCurrentProgram(int index)
{
}

const String GuitarPluginAudioProcessor::getProgramName(int index)
{
    return {};
}

void GuitarPluginAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void GuitarPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    reverbProcessor.setSampleRate(sampleRate);

    stopSignal = false;
    cap.open(0);
    if (!cap.isOpened())
    {
        DBG("Error: Could not open webcam.");
    }
    else
    {
        std::thread([this] { webcamLoop(); }).detach();
    }
}

void GuitarPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    stopSignal = true;
    if (cap.isOpened())
    {
        cap.release();
    }
    cv::destroyAllWindows();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

# define M_PI 3.14159265358979323846  /* pi */

void GuitarPluginAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float distortion_drive = *state->getRawParameterValue("distortion_driveAttachment");
    float distortion_range = *state->getRawParameterValue("distortion_rangeAttachment");
    float distortion_blend = *state->getRawParameterValue("distortion_blendAttachment");
    float distortion_volume = *state->getRawParameterValue("distortion_volumeAttachment");

    reverbParams.roomSize = *state->getRawParameterValue("reverb_roomSizeAttachment");
    float reverb_wetDry = *state->getRawParameterValue("reverb_wetDryAttachment");
    reverbParams.damping = *state->getRawParameterValue("reverb_dampingAttachment");
    reverbParams.width = *state->getRawParameterValue("reverb_widthAttachment");

    reverbParams.wetLevel = reverb_wetDry;
    reverbParams.dryLevel = 1.0f - reverb_wetDry;

    distortion_volume += 10;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            float cleanSig = *channelData;

            *channelData *= (distortion_range * 0.1f);

            float dirtySig = (2.f / M_PI) * atan(*channelData);

            *channelData = distortion_volume * (((dirtySig * distortion_blend) + (cleanSig * (1.f - distortion_blend))) / 2.f);

            channelData++;
        }
    }

    reverbProcessor.setParameters(reverbParams);
    reverbProcessor.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
}

//==============================================================================
bool GuitarPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GuitarPluginAudioProcessor::createEditor()
{
    return new GuitarPluginAudioProcessorEditor(*this);
}

//==============================================================================
void GuitarPluginAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);
}

void GuitarPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    ValueTree tree = ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid()) {
        state->state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    ScopedPointer<AudioProcessorValueTreeState> state;

    return new GuitarPluginAudioProcessor();
}

void GuitarPluginAudioProcessor::webcamLoop()
{
    while (!stopSignal)
    {
        cap >> frame;
        if (!frame.empty())
        {
            cv::imshow("Webcam", frame);
            cv::waitKey(1);
        }
    }
}


AudioProcessorValueTreeState& GuitarPluginAudioProcessor::getState() {
    return *state;
}