/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <opencv2/opencv.hpp>
#include "PluginProcessor.h"
using namespace juce;

//==============================================================================
/**
*/
class GuitarPluginAudioProcessorEditor : public AudioProcessorEditor
{
public:
    GuitarPluginAudioProcessorEditor(GuitarPluginAudioProcessor&);
    ~GuitarPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

private:
    //distortion
    ScopedPointer<Slider> distortion_driveKnob;
    ScopedPointer<Slider> distortion_rangeKnob;
    ScopedPointer<Slider> distortion_blendKnob;
    ScopedPointer<Slider> distortion_volumeKnob;

    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> distortion_driveAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> distortion_rangeAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> distortion_blendAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> distortion_volumeAttachment;


    //reverb
    ScopedPointer<Slider> reverb_roomSizeKnob;
    ScopedPointer<Slider> reverb_wetDryKnob;
    ScopedPointer<Slider> reverb_dampingKnob;
    ScopedPointer<Slider> reverb_widthKnob;

    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> reverb_roomSizeAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> reverb_wetDryAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> reverb_dampingAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> reverb_widthAttachment;


    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarPluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarPluginAudioProcessorEditor)
};