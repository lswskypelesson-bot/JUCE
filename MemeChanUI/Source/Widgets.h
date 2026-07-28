#pragma once

#include "Theme.h"

/*  Reusable controls for the MemeChan editor.

    These are ordinary juce::Slider / juce::Button / juce::ComboBox subclasses so
    that they can be driven straight from an AudioProcessorValueTreeState via the
    usual SliderAttachment / ButtonAttachment / ComboBoxAttachment once the
    processor side is wired up.
*/

using IconFn = std::function<juce::Path (juce::Rectangle<float>)>;

//==============================================================================
class MemeChanLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    MemeChanLookAndFeel();

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;
};

//==============================================================================
/** Rotary control with the photographic "paw" face. The face is a placeholder
    shape here; production builds swap in a film-strip image of the artwork. */
class PawKnob final : public juce::Slider
{
public:
    PawKnob();
    void paint (juce::Graphics&) override;
};

//==============================================================================
/** Knob with a caption above and a formatted read-out below. */
class LabelledKnob final : public juce::Component
{
public:
    using Formatter = std::function<juce::String (double)>;

    LabelledKnob (juce::String caption, juce::Range<double> range, double initialValue,
                  Formatter, float knobDiameter);

    void paint (juce::Graphics&) override;
    void resized() override;

    PawKnob knob;

private:
    juce::String captionText;
    Formatter format;
    float diameter;
};

//==============================================================================
/** Knob with the read-out and name stacked to its right, as used by the FX row. */
class FxKnobCell final : public juce::Component
{
public:
    FxKnobCell (juce::String name, juce::String valueText, double proportion);

    void paint (juce::Graphics&) override;
    void resized() override;

    PawKnob knob;

private:
    juce::String nameText, valueString;
};

//==============================================================================
class LevelMeter final : public juce::Component
{
public:
    explicit LevelMeter (bool scaleOnRightSide);

    /** Feed from the audio thread via an atomic; the editor timer reads it. */
    void setLevelDb (float db);
    void paint (juce::Graphics&) override;

private:
    bool scaleRight;
    float levelDb = -20.0f;
};

//==============================================================================
class FlatButton final : public juce::Button
{
public:
    enum class Style { plain, outlined, filled, ghost, accentText };

    FlatButton (juce::String text, Style, IconFn icon = {});

    void paintButton (juce::Graphics&, bool highlighted, bool down) override;

    void setIconColour (juce::Colour c) { iconColour = c; }
    void setFontHeight (float h)        { fontHeight = h; }

private:
    Style style;
    IconFn iconFn;
    juce::Colour iconColour { theme::colours::textPrimary };
    float fontHeight = 13.0f;
};

//==============================================================================
class SegmentedControl final : public juce::Component
{
public:
    SegmentedControl (juce::StringArray options, int selected);

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;

private:
    juce::StringArray items;
    int selectedIndex;
};

//==============================================================================
class TabStrip final : public juce::Component
{
public:
    struct Tab { juce::String name; IconFn icon; };

    TabStrip (std::vector<Tab>, int selected);

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;

    void setSelectedTab (int index);
    int getSelectedTab() const noexcept { return selectedIndex; }

    std::function<void (int)> onTabChange;

private:
    juce::Rectangle<float> boundsForTab (int index) const;

    std::vector<Tab> tabs;
    int selectedIndex;
};

//==============================================================================
/** Horizontal row of effect "pills" joined by arrows. */
class FxChainStrip final : public juce::Component
{
public:
    struct Item { juce::String name, value; juce::Colour dot; };

    FxChainStrip (std::vector<Item>, bool compact);

    void paint (juce::Graphics&) override;

private:
    std::vector<Item> items;
    bool compactStyle;
};

//==============================================================================
/** Static waveform preview for the input-calibration panel. */
class WaveformView final : public juce::Component
{
public:
    WaveformView();
    void paint (juce::Graphics&) override;

private:
    juce::Array<float> samples;
};
