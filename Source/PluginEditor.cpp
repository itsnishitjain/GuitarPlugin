/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include <opencv2/opencv.hpp>
#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace juce;

//==============================================================================
GuitarPluginAudioProcessorEditor::GuitarPluginAudioProcessorEditor(GuitarPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    //distortion
    addAndMakeVisible(distortion_driveKnob = new Slider("distortion_DriveSlider"));
    distortion_driveKnob->setSliderStyle(Slider::Rotary);
    distortion_driveKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(distortion_rangeKnob = new Slider("distortion_RangeSlider"));
    distortion_rangeKnob->setSliderStyle(Slider::Rotary);
    distortion_rangeKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(distortion_blendKnob = new Slider("distortion_BlendSlider"));
    distortion_blendKnob->setSliderStyle(Slider::Rotary);
    distortion_blendKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(distortion_volumeKnob = new Slider("distortion_VolumeSlider"));
    distortion_volumeKnob->setSliderStyle(Slider::Rotary);
    distortion_volumeKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    distortion_driveAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "distortion_driveAttachment", *distortion_driveKnob);
    distortion_rangeAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "distortion_rangeAttachment", *distortion_rangeKnob);
    distortion_blendAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "distortion_blendAttachment", *distortion_blendKnob);
    distortion_volumeAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "distortion_volumeAttachment", *distortion_volumeKnob);


    //reverb
    addAndMakeVisible(reverb_roomSizeKnob = new Slider("reverb_roomSizeSlider"));
    reverb_roomSizeKnob->setSliderStyle(Slider::Rotary);
    reverb_roomSizeKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(reverb_wetDryKnob = new Slider("distortion_WetDrySlider"));
    reverb_wetDryKnob->setSliderStyle(Slider::Rotary);
    reverb_wetDryKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(reverb_dampingKnob = new Slider("distortion_DampingSlider"));
    reverb_dampingKnob->setSliderStyle(Slider::Rotary);
    reverb_dampingKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(reverb_widthKnob = new Slider("distortion_WidthSlider"));
    reverb_widthKnob->setSliderStyle(Slider::Rotary);
    reverb_widthKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    reverb_roomSizeAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "reverb_roomSizeAttachment", *reverb_roomSizeKnob);
    reverb_wetDryAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "reverb_wetDryAttachment", *reverb_wetDryKnob);
    reverb_dampingAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "reverb_dampingAttachment", *reverb_dampingKnob);
    reverb_widthAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "reverb_widthAttachment", *reverb_widthKnob);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(1000, 600);
}

GuitarPluginAudioProcessorEditor::~GuitarPluginAudioProcessorEditor()
{
}

//==============================================================================
void GuitarPluginAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);
    g.setFont(15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);

    //distortion
    g.drawText("Drive", (getWidth() / 20), (getHeight() / 5) * 1 + 5, 100, 100, Justification::centred, false);
    g.drawText("Range", (getWidth() / 20), (getHeight() / 5) * 2 + 5, 100, 100, Justification::centred, false);
    g.drawText("Blend", (getWidth() / 20), (getHeight() / 5) * 3 + 5, 100, 100, Justification::centred, false);
    g.drawText("Volume", (getWidth() / 20), (getHeight() / 5) * 4 + 5, 100, 100, Justification::centred, false);

    //reverb
    g.drawText("roomSize", (getWidth() / 5), (getHeight() / 5) * 1 + 5, 100, 100, Justification::centred, false);
    g.drawText("wet/dry", (getWidth() / 5), (getHeight() / 5) * 2 + 5, 100, 100, Justification::centred, false);
    g.drawText("Damping", (getWidth() / 5), (getHeight() / 5) * 3 + 5, 100, 100, Justification::centred, false);
    g.drawText("Width", (getWidth() / 5), (getHeight() / 5) * 4 + 5, 100, 100, Justification::centred, false);
}

void GuitarPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    distortion_driveKnob->setBounds((getWidth() / 20), ((getHeight() / 5) * 1) - (100 / 2), 100, 100);
    distortion_rangeKnob->setBounds((getWidth() / 20), ((getHeight() / 5) * 2) - (100 / 2), 100, 100);
    distortion_blendKnob->setBounds((getWidth() / 20), ((getHeight() / 5) * 3) - (100 / 2), 100, 100);
    distortion_volumeKnob->setBounds((getWidth() / 20), ((getHeight() / 5) * 4) - (100 / 2), 100, 100);

    reverb_roomSizeKnob->setBounds((getWidth() / 5), ((getHeight() / 5) * 1) - (100 / 2), 100, 100);
    reverb_wetDryKnob->setBounds((getWidth() / 5), ((getHeight() / 5) * 2) - (100 / 2), 100, 100);
    reverb_dampingKnob->setBounds((getWidth() / 5), ((getHeight() / 5) * 3) - (100 / 2), 100, 100);
    reverb_widthKnob->setBounds((getWidth() / 5), ((getHeight() / 5) * 4) - (100 / 2), 100, 100);

}