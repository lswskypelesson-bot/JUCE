#pragma once

#include "HeroBanner.h"
#include "Pages.h"

/*  Full editor layout for MemeChan Screaming!! DELUXE.

    Design size is 1448 x 1086; resized() works from proportional bounds off a
    single scale factor so the window can be resized later without reflowing
    anything by hand.

    No processor is attached yet - every control holds a static value so the
    layout can be reviewed. Wiring is a matter of adding the matching
    AudioProcessorValueTreeState attachments next to each control.
*/
class MemeChanEditor final : public juce::Component
{
public:
    static constexpr int designWidth  = 1448;
    static constexpr int designHeight = 1086;

    MemeChanEditor();
    ~MemeChanEditor() override;

    void paint (juce::Graphics&) override;
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;

    /** Switches tab; used by the offline renderer to capture every page. */
    void showPage (int index);

private:
    void paintTopBar (juce::Graphics&);
    void paintBottomBar (juce::Graphics&);

    MemeChanLookAndFeel lnf;

    // header ------------------------------------------------------------
    HeroBanner hero;
    std::unique_ptr<juce::Component> presetBar;
    FlatButton saveButton    { "SAVE",    FlatButton::Style::plain };
    FlatButton saveAsButton  { "SAVE AS", FlatButton::Style::plain };
    FlatButton moreButton    { juce::String::fromUTF8 ("\xe2\x80\xa2\xe2\x80\xa2\xe2\x80\xa2"),
                               FlatButton::Style::plain };
    FlatButton inputButton  { "INPUT",  FlatButton::Style::plain, theme::waveIcon };
    FlatButton filterButton { "FILTER", FlatButton::Style::plain, theme::funnelIcon };
    FlatButton gearButton   { "",       FlatButton::Style::plain, theme::gearIcon };

    // hero --------------------------------------------------------------
    std::unique_ptr<juce::Component> inputCard, outputCard, heroKnobPanel;

    // body --------------------------------------------------------------
    std::unique_ptr<TabStrip> tabs;
    std::vector<std::unique_ptr<PagePanel>> pages;

    // footer ------------------------------------------------------------
    juce::ComboBox oversamplingBox, presetInitBox;

    // geometry, in design units; paint() applies `scale`
    float scale = 1.0f;
    juce::Rectangle<int> mainPanelBounds { 22, 638, 1404, 390 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MemeChanEditor)
};
