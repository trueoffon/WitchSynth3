#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include <functional>
#include <unordered_map>

class WitchLookAndFeel : public juce::LookAndFeel_V4
{
public:
    WitchLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override;
};

class WitchXYPad : public juce::Component
{
public:
    explicit WitchXYPad(juce::AudioProcessorValueTreeState& state);
    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;

private:
    void updateFromMouse(const juce::MouseEvent&);
    juce::AudioProcessorValueTreeState& apvts;
};

class WitchStepMatrix : public juce::Component
{
public:
    WitchStepMatrix(juce::AudioProcessorValueTreeState& state, std::atomic<int>& stepRef);
    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    std::atomic<int>& activeStep;
};

class WitchListModel : public juce::ListBoxModel
{
public:
    std::vector<juce::String> items;
    std::function<void(int)> onRowClick;

    int getNumRows() override { return static_cast<int>(items.size()); }
    void paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override
    {
        if (onRowClick) onRowClick(row);
    }
};

class WitchSynthAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit WitchSynthAudioProcessorEditor(WitchSynthAudioProcessor&);
    ~WitchSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseEnter(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&) override;

private:
    enum class SidePage { Main = 0, Osc, Arp, XY };

    void timerCallback() override;
    void setSidePage(SidePage page);
    void updateVisibility();
    void rebuildPresetListForCategory(int uiCategoryIdx);
    void syncPresetUI();
    void updateLikeButtonVisual();
    void randomizeWitchPreset();

    WitchSynthAudioProcessor& audioProcessor;
    WitchLookAndFeel lnf;
    WitchXYPad xyPad;
    WitchStepMatrix stepMatrix;
    WitchListModel categoryModel, presetModel;
    juce::ListBox categoryList{ "CatList", &categoryModel };
    juce::ListBox presetList{ "PresetList", &presetModel };

    std::vector<int> visiblePresetIndices;
    int selectedUiCategory = 1;
    SidePage currentPage = SidePage::Main;
    int lastSeenPreset = -1;
    float smoothedPeak = 0.0f;
    juce::String activeHelpText;

    // Header & Screen Controls
    juce::TextButton prevBtn{ "<" }, nextBtn{ ">" }, likeBtn{ "LIKE" };
    juce::TextButton rndBtn{ "RND" }, initBtn{ "INIT" }, lockFxBtn{ "LOCK" }, panicBtn{ "PANIC" };
    juce::TextButton pageMainBtn{ "MAIN" }, pageOscBtn{ "OSC" }, pageArpBtn{ "ARP" }, pageXyBtn{ "XY" };

    // Side Panels Controls
    juce::ComboBox filterTypeBox, polyModeBox, morphPresetBox;
    juce::Slider filterCutoff, filterReso, filterDrive, filterEnvAmt, filterDecay, filterKeyTrack, tiltAmt, stereoWidth;
    juce::Slider ampA, ampD, ampS, ampR, glideTime, snapAmt, airAmt, morphAmt;

    juce::ComboBox osc1WaveBox, osc2WaveBox;
    juce::Slider osc1Oct, osc1Semi, pwmWidth, xModAmt, osc1Detune, osc1Level;
    juce::Slider osc2Oct, osc2Semi, osc2Unison, osc2Level, subLevel, entropyAmt, gravityAmt, ghostVoxAmt;

    juce::ComboBox lfoTargetBox, lfoShapeBox, arpModeBox;
    juce::Slider lfoRate, lfoDepth, arpRate, pitchPunch, pumpAmt, noiseLevel, vhsDrift;

    // Bottom Separated FX Controls (always visible)
    juce::ToggleButton distBtn{ "DIST" }, chorusBtn{ "CHORUS" }, delayBtn{ "DELAY" }, reverbBtn{ "REVERB" }, freezeBtn{ "FREEZE" }, tapeStopBtn{ "T.STOP" };
    juce::ComboBox distModeBox, delayModeBox;
    juce::Slider distDrive, distMix, crushAmount, lowCutHz;
    juce::Slider chorusMix, chorusRate, chorusDepth;
    juce::Slider delayTime, delayFb, delayMix;
    juce::Slider reverbSize, reverbMix, haloAmt, masterGain;

    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAtt  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using BtnAtt    = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::vector<std::unique_ptr<SliderAtt>> sliderAtts;
    std::vector<std::unique_ptr<ComboAtt>> comboAtts;
    std::vector<std::unique_ptr<BtnAtt>> btnAtts;
    std::unordered_map<juce::Component*, std::unique_ptr<juce::Label>> knobLabels;
    std::unordered_map<const juce::Component*, juce::String> helpMap;

    void registerHelp(juce::Component* c, const juce::String& ruHelp);
    void initKnob(juce::Slider& s, const juce::String& id, const juce::String& name, const juce::String& ruHelp, bool attachLbl = true);
    void initCombo(juce::ComboBox& c, const juce::String& id, const juce::StringArray& items, const juce::String& ruHelp);
    void initToggle(juce::ToggleButton& b, const juce::String& id, const juce::String& ruHelp);
    void showCtrl(juce::Component& c, bool visible);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WitchSynthAudioProcessorEditor)
};
