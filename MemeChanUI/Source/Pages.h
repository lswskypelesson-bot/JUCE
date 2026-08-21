#pragma once

#include "Widgets.h"

/*  Tab pages.

    A page is described rather than hand-coded: a handful of add*() calls place
    a card and its controls together, so the four tabs stay on the same grid and
    a new one is a dozen lines. All coordinates are page-relative and in design
    units - MemeChanEditor applies the window scale.
*/
class PagePanel : public juce::Component
{
public:
    struct FxCellSpec { juce::String name, value; double proportion; bool enabled; };

    PagePanel();

    void paint (juce::Graphics&) override;

    //== card builders, one per row-1 / row-2 slot ============================
    juce::ComboBox& addCombo     (juce::Rectangle<int> card, juce::String title,
                                  juce::StringArray items, int selectedIndex);
    void            addKnobRow   (juce::Rectangle<int> card, juce::String title,
                                  juce::StringArray names, std::vector<double> values);
    void            addBigKnob   (juce::Rectangle<int> card, juce::String title,
                                  double value, juce::Range<double> range,
                                  std::function<juce::String (double)>);
    void            addSegmented (juce::Rectangle<int> card, juce::String title,
                                  juce::StringArray options, int selectedIndex);
    void            addSlider    (juce::Rectangle<int> card, juce::String title,
                                  double value, double maximum, juce::String readout);
    void            addWaveButton(juce::Rectangle<int> card, juce::String title,
                                  juce::String buttonText);

    /** Card with its controls deliberately left out, for a section that is
        parked rather than removed. */
    void            addBlankCard (juce::Rectangle<int> card, juce::String title,
                                  juce::String note);

    /** Free-standing caption, e.g. a note under a control. */
    void            addNote      (juce::Rectangle<int> area, juce::String text,
                                  float size = 11.0f);

    //== the shared lower half ================================================
    void addFxSection (juce::String caption, std::vector<FxChainStrip::Item> chain,
                       std::vector<FxCellSpec> cells);
    void addSideBox   (juce::String title, juce::String rowLabel,
                       juce::StringArray comboItems, int selectedIndex,
                       juce::String innerLabel, juce::StringArray innerChain);

    void addCatWatermark (juce::Rectangle<int>);

    //== standard slot geometry ==============================================
    static juce::Rectangle<int> row1 (int slot);   // 0..2
    static juce::Rectangle<int> row2 (int slot);   // 0..2

private:
    struct Box  { juce::Rectangle<int> bounds; float corner; juce::Colour fill, border; };
    struct Text { juce::Rectangle<int> bounds; juce::String text; float size; bool bold;
                  juce::Colour colour; juce::Justification just; };

    void addCard (juce::Rectangle<int>, const juce::String& title);
    template <typename T> T& own (std::unique_ptr<T>, juce::Rectangle<int>);

    std::vector<Box> boxes;
    std::vector<Text> texts;
    std::vector<juce::Rectangle<int>> watermarks;
    std::vector<std::unique_ptr<juce::Component>> owned;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PagePanel)
};

//==============================================================================
std::unique_ptr<PagePanel> createAmpPage();
std::unique_ptr<PagePanel> createDrivePage();
std::unique_ptr<PagePanel> createFxPage();
std::unique_ptr<PagePanel> createRoutingPage();
