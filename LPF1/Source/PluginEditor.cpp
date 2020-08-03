/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Lpf1AudioProcessorEditor::Lpf1AudioProcessorEditor (Lpf1AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    // these define the parameters of our slider object
    cutoff.setSliderStyle (Slider::LinearBarVertical);
    cutoff.setRange(20.0, 20.0e3, 1.0);
    cutoff.setTextBoxStyle (Slider::NoTextBox, false, 90, 0);
    cutoff.setPopupDisplayEnabled (true, false, this);
    cutoff.setTextValueSuffix (" Hz");
    cutoff.setValue(1.0e3);
    // this function adds the slider to the editor
    addAndMakeVisible (&cutoff);
    
    cutoff.addListener (this);
    
    // these define the parameters of our slider object
    gain.setSliderStyle (Slider::LinearBarVertical);
    gain.setRange(-20.0, 20.0, 0.5);
    gain.setTextBoxStyle (Slider::NoTextBox, false, 90, 0);
    gain.setPopupDisplayEnabled (true, false, this);
    gain.setTextValueSuffix (" dB");
    gain.setValue(0.0);
    // this function adds the slider to the editor
    addAndMakeVisible (&gain);
    
    gain.addListener (this);
    
    // these define the parameters of our slider object
    Q.setSliderStyle (Slider::LinearBarVertical);
    Q.setRange(0.1, 100.0, 0.1);
    Q.setTextBoxStyle (Slider::NoTextBox, false, 90, 0);
    Q.setPopupDisplayEnabled (true, false, this);
    Q.setValue(0.707);
    // this function adds the slider to the editor
    addAndMakeVisible (&Q);
    
    Q.addListener (this);
}

Lpf1AudioProcessorEditor::~Lpf1AudioProcessorEditor()
{
}

//==============================================================================
void Lpf1AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Cutoff Frequency", 10, 3, 90, 10, Justification::left, 1);
    g.drawFittedText ("Gain", 180, 3, 90, 10, Justification::centred, 1);
    g.drawFittedText ("Resonance", 330, 3, 90, 10, Justification::right, 1);
}

void Lpf1AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    cutoff.setBounds (40, 30, 20, getHeight() - 60);
    gain.setBounds (190, 30, 20, getHeight() - 60);
    Q.setBounds (340, 30, 20, getHeight() - 60);
}

void Lpf1AudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    processor.cutoffVal = cutoff.getValue();
    processor.gainVal = gain.getValue();
    processor.qVal = Q.getValue();
}
