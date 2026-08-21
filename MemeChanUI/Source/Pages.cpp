#include "Pages.h"

using namespace theme;

namespace
{
    juce::String oneDecimal (double v) { return juce::String (v, 1); }

    const juce::Colour fxDots[] = { colours::accent,
                                    juce::Colour (0xffe0a23b),
                                    juce::Colour (0xff3bc0e0),
                                    juce::Colour (0xff8b5fe0),
                                    juce::Colour (0xff3be08b) };
}

//==============================================================================
PagePanel::PagePanel() {}

juce::Rectangle<int> PagePanel::row1 (int slot)
{
    static const juce::Rectangle<int> slots[] = { { 18,   18, 330, 126 },
                                                  { 364,  18, 700, 126 },
                                                  { 1080, 18, 306, 126 } };
    return slots[juce::jlimit (0, 2, slot)];
}

juce::Rectangle<int> PagePanel::row2 (int slot)
{
    static const juce::Rectangle<int> slots[] = { { 18,  156, 546, 76 },
                                                  { 580, 156, 402, 76 },
                                                  { 998, 156, 388, 76 } };
    return slots[juce::jlimit (0, 2, slot)];
}

template <typename T>
T& PagePanel::own (std::unique_ptr<T> c, juce::Rectangle<int> bounds)
{
    auto& ref = *c;
    ref.setBounds (bounds);
    addAndMakeVisible (ref);
    owned.push_back (std::move (c));
    return ref;
}

void PagePanel::addCard (juce::Rectangle<int> card, const juce::String& title)
{
    boxes.push_back ({ card, 12.0f, colours::panelBg, colours::cardBorder });

    if (title.isNotEmpty())
        texts.push_back ({ { card.getX() + 16, card.getY() + 12, card.getWidth() - 32, 15 },
                           title, 11.0f, true, colours::textSecondary,
                           juce::Justification::centredLeft });
}

void PagePanel::addCatWatermark (juce::Rectangle<int> r) { watermarks.push_back (r); }

//==============================================================================
juce::ComboBox& PagePanel::addCombo (juce::Rectangle<int> card, juce::String title,
                                     juce::StringArray items, int selectedIndex)
{
    addCard (card, title);

    auto box = std::make_unique<juce::ComboBox>();
    box->addItemList (items, 1);
    box->setSelectedId (selectedIndex + 1, juce::dontSendNotification);

    return own (std::move (box), { card.getX() + 16, card.getY() + 44,
                                   card.getWidth() - 32, 40 });
}

void PagePanel::addKnobRow (juce::Rectangle<int> card, juce::String title,
                            juce::StringArray names, std::vector<double> values)
{
    addCard (card, title);

    // columns keep a sane width when a card holds fewer than four knobs
    auto area = card.reduced (16, 0).withTrimmedLeft (64);
    const auto colW = juce::jmin (160, area.getWidth() / juce::jmax (1, names.size()));

    for (int i = 0; i < names.size(); ++i)
        own (std::make_unique<LabelledKnob> (names[i], juce::Range<double> (0.0, 10.0),
                                             values[(size_t) i], oneDecimal, 56.0f),
             { area.getX() + i * colW, card.getY() + 12, colW, 102 });
}

void PagePanel::addBigKnob (juce::Rectangle<int> card, juce::String title, double value,
                            juce::Range<double> range, std::function<juce::String (double)> fmt)
{
    addCard (card, title);

    own (std::make_unique<LabelledKnob> ("", range, value, std::move (fmt), 56.0f),
         { card.getX() + 16, card.getY() + 20, card.getWidth() - 32, card.getHeight() - 32 });
}

void PagePanel::addSegmented (juce::Rectangle<int> card, juce::String title,
                              juce::StringArray options, int selectedIndex)
{
    addCard (card, title);

    own (std::make_unique<SegmentedControl> (options, selectedIndex),
         { card.getX() + 16, card.getY() + 30, card.getWidth() - 32, 32 });
}

void PagePanel::addSlider (juce::Rectangle<int> card, juce::String title,
                           double value, double maximum, juce::String readout)
{
    addCard (card, title);

    auto s = std::make_unique<juce::Slider>();
    s->setSliderStyle (juce::Slider::LinearHorizontal);
    s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    s->setRange (0.0, maximum, 0.0);
    s->setValue (value, juce::dontSendNotification);

    own (std::move (s), { card.getX() + 16, card.getY() + 28, card.getWidth() - 96, 32 });

    texts.push_back ({ { card.getRight() - 72, card.getY() + 12, 56, card.getHeight() - 12 },
                       readout, 12.5f, true, colours::textSecondary,
                       juce::Justification::centredRight });
}

void PagePanel::addWaveButton (juce::Rectangle<int> card, juce::String title,
                               juce::String buttonText)
{
    addCard (card, title);

    own (std::make_unique<WaveformView>(),
         { card.getX() + 16, card.getY() + 28, 300, 32 });

    own (std::make_unique<FlatButton> (buttonText, FlatButton::Style::outlined),
         { card.getX() + 360, card.getY() + 26, 170, 36 });
}

void PagePanel::addBlankCard (juce::Rectangle<int> card, juce::String title, juce::String note)
{
    addCard (card, title);

    texts.push_back ({ card.withTrimmedTop (20), note, 13.0f, false, colours::textTertiary,
                       juce::Justification::centred });
}

void PagePanel::addNote (juce::Rectangle<int> area, juce::String text, float size)
{
    texts.push_back ({ area, text, size, false, colours::textTertiary,
                       juce::Justification::centredLeft });
}

//==============================================================================
void PagePanel::addFxSection (juce::String caption, std::vector<FxChainStrip::Item> chain,
                              std::vector<FxCellSpec> cells)
{
    texts.push_back ({ { 18, 244, 90, 40 }, caption, 11.0f, true, colours::textSecondary,
                       juce::Justification::centredLeft });

    own (std::make_unique<FxChainStrip> (std::move (chain), false), { 108, 244, 960, 40 });

    for (size_t i = 0; i < cells.size(); ++i)
    {
        const auto x = 18 + (int) i * 210;

        own (std::make_unique<FxKnobCell> (cells[i].name, cells[i].value, cells[i].proportion),
             { x, 294, 200, 52 });

        auto b = std::make_unique<FlatButton> (cells[i].name,
                                               cells[i].enabled ? FlatButton::Style::accentText
                                                                : FlatButton::Style::plain);
        b->setFontHeight (11.5f);
        own (std::move (b), { x, 352, 96, 28 });

        if (i + 1 < cells.size())
        {
            auto dots = std::make_unique<FlatButton> (
                juce::String::fromUTF8 ("\xe2\x80\xa2\xe2\x80\xa2\xe2\x80\xa2"),
                FlatButton::Style::plain);
            dots->setFontHeight (11.0f);
            own (std::move (dots), { x + 102, 352, 40, 28 });
        }
    }
}

void PagePanel::addSideBox (juce::String title, juce::String rowLabel,
                            juce::StringArray comboItems, int selectedIndex,
                            juce::String innerLabel, juce::StringArray innerChain)
{
    const juce::Rectangle<int> box { 1080, 238, 306, 142 };

    boxes.push_back ({ box, 12.0f, colours::cardBg, colours::accent });
    boxes.push_back ({ { box.getX() + 12, box.getY() + 82, box.getWidth() - 24, 48 },
                       10.0f, colours::panelBg, colours::cardBorder });

    texts.push_back ({ { box.getX() + 16, box.getY() + 12, 160, 16 }, title,
                       13.5f, true, colours::textPrimary, juce::Justification::centredLeft });
    texts.push_back ({ { box.getX() + 16, box.getY() + 44, 70, 20 }, rowLabel,
                       11.0f, true, colours::textSecondary, juce::Justification::centredLeft });
    texts.push_back ({ { box.getX() + 20, box.getY() + 84, 80, 14 }, innerLabel,
                       9.5f, true, colours::textTertiary, juce::Justification::centredLeft });

    auto combo = std::make_unique<juce::ComboBox>();
    combo->addItemList (comboItems, 1);
    combo->setSelectedId (selectedIndex + 1, juce::dontSendNotification);
    own (std::move (combo), { box.getX() + 92, box.getY() + 40, 200, 32 });

    std::vector<FxChainStrip::Item> compact;

    for (const auto& n : innerChain)
        compact.push_back ({ n, {}, {} });

    own (std::make_unique<FxChainStrip> (std::move (compact), true),
         { box.getX() + 20, box.getY() + 100, 266, 26 });
}

//==============================================================================
void PagePanel::paint (juce::Graphics& g)
{
    for (const auto& b : boxes)
        drawCard (g, b.bounds.toFloat(), b.corner, b.fill, b.border);

    for (const auto& w : watermarks)
        drawCat (g, w.toFloat(), colours::catBlack.withAlpha (0.08f));

    for (const auto& t : texts)
    {
        g.setColour (t.colour);
        g.setFont (sans (t.size, t.bold));
        g.drawText (t.text, t.bounds, t.just);
    }
}

//==============================================================================
/*  The six amp captures share one amp and differ only in the cabinet baked
    into them, so the selector is a cab list, not an amp list. Names are
    placeholders until the real .nam filenames are known. */
static juce::StringArray cabCaptures()
{
    return { "JK Clean - IR 1", "JK Clean - IR 2", "JK Clean - IR 3",
             "JK Clean - IR 4", "JK Clean - IR 5", "JK Clean - IR 6" };
}

std::unique_ptr<PagePanel> createAmpPage()
{
    auto p = std::make_unique<PagePanel>();

    // amp is fixed; the six captures differ only in the cab baked into them
    p->addCombo (PagePanel::row1 (0), "CAB (BAKED IR)", cabCaptures(), 0);
    p->addNote  ({ 34, 104, 300, 16 }, "amp: JK Clean - capture includes cab");
    p->addCatWatermark ({ 262, 100, 60, 34 });

    p->addBlankCard (PagePanel::row1 (1), "EQ", "parked");

    p->addBigKnob (PagePanel::row1 (2), "NOISE GATE", -60.0, { -80.0, 0.0 },
                   [] (double v) { return juce::String (v, 1) + " dB"; });

    p->addWaveButton (PagePanel::row2 (0), "INPUT CALIBRATION", "CALIBRATE");
    p->addBlankCard  (PagePanel::row2 (1), "PICKUP", "parked");
    p->addBlankCard  (PagePanel::row2 (2), "BASS CUT", "filter off");

    p->addFxSection ("FX CHAIN",
                     { { "ROOM",     "45%", fxDots[0] },
                       { "MONO DLY", "50%", fxDots[1] },
                       { "DELAY",    "50%", fxDots[2] },
                       { "HALL",     "79%", fxDots[3] },
                       { "DRY PAN",  "C",   fxDots[4] } },
                     { { "ROOM",     "45.0 %", 0.45,  true  },
                       { "MONO DLY", "50.0 %", 0.50,  false },
                       { "DELAY",    "50.0 %", 0.50,  false },
                       { "HALL",     "78.8 %", 0.788, true  },
                       { "DRY PAN",  "C",      0.50,  false } });

    p->addSideBox ("NAM CHAIN", "RATE", { "48 kHz", "44.1 kHz" }, 0, "STAGES",
                   { "IN", "GATE", "OD", "AMP+IR", "OUT" });
    return p;
}

std::unique_ptr<PagePanel> createDrivePage()
{
    auto p = std::make_unique<PagePanel>();

    p->addCombo (PagePanel::row1 (0), "OD MODEL", { "OD_MIKE" }, 0);
    p->addNote  ({ 34, 104, 300, 16 }, "runs into the amp capture");
    p->addCatWatermark ({ 262, 100, 60, 34 });

    // DRIVE and TONE are baked into the capture, so only the levels are live
    p->addKnobRow (PagePanel::row1 (1), "OD", { "LEVEL", "MIX" }, { 5.5, 10.0 });
    p->addNote    ({ 800, 30, 250, 16 }, "drive / tone fixed by the capture");

    p->addBigKnob (PagePanel::row1 (2),
                   juce::String::fromUTF8 ("OD \xe2\x86\x92 AMP"), 0.0, { -12.0, 12.0 },
                   [] (double v) { return (v >= 0.0 ? "+" : "") + juce::String (v, 1) + " dB"; });

    p->addSegmented (PagePanel::row2 (0), "OD STAGE", { "ON", "BYPASS" }, 0);
    p->addBlankCard (PagePanel::row2 (1), "DRIVE", "baked into capture");
    p->addBlankCard (PagePanel::row2 (2), "TONE",  "baked into capture");

    p->addFxSection ("STACK",
                     { { "IN",     "0dB",   fxDots[0] },
                       { "GATE",   "-60dB", fxDots[1] },
                       { "OD",     "ON",    fxDots[2] },
                       { "AMP+IR", "IR 1",  fxDots[3] },
                       { "OUT",    "0dB",   fxDots[4] } },
                     { { "OD LEVEL", "55.0 %",  0.55, true  },
                       { "OD MIX",   "100.0 %", 1.00, true  },
                       { "STAGE",    "0.0 dB",  0.50, true  },
                       { "GATE",     "-60 dB",  0.25, false },
                       { "OUT",      "0.0 dB",  0.50, false } });

    p->addSideBox ("OD MODEL", "RATE", { "48 kHz", "44.1 kHz" }, 0, "LOADED",
                   { "OD_MIKE", "48kHz", "READY" });
    return p;
}

std::unique_ptr<PagePanel> createFxPage()
{
    auto p = std::make_unique<PagePanel>();

    p->addCombo (PagePanel::row1 (0), "SLOT",
                 { "Hall", "Room", "Plate", "Spring" }, 0);
    p->addCatWatermark ({ 250, 104, 66, 38 });

    p->addKnobRow (PagePanel::row1 (1), "REVERB",
                   { "SIZE", "DAMP", "PRE-DLY", "MIX" }, { 7.8, 4.2, 2.5, 3.6 });

    p->addBigKnob (PagePanel::row1 (2), "DUCKING", -12.0, { -40.0, 0.0 },
                   [] (double v) { return juce::String (v, 1) + " dB"; });

    p->addSlider    (PagePanel::row2 (0), "DELAY TIME", 375.0, 2000.0, "375 ms");
    p->addSegmented (PagePanel::row2 (1), "SYNC", { "FREE", "TEMPO" }, 1);
    p->addSlider    (PagePanel::row2 (2), "FEEDBACK", 38.0, 100.0, "38 %");

    p->addFxSection ("FX CHAIN",
                     { { "ROOM",     "45%", fxDots[0] },
                       { "MONO DLY", "50%", fxDots[1] },
                       { "DELAY",    "50%", fxDots[2] },
                       { "HALL",     "79%", fxDots[3] },
                       { "DRY PAN",  "C",   fxDots[4] } },
                     { { "ROOM",     "45.0 %", 0.45,  true  },
                       { "MONO DLY", "50.0 %", 0.50,  false },
                       { "DELAY",    "50.0 %", 0.50,  false },
                       { "HALL",     "78.8 %", 0.788, true  },
                       { "DRY PAN",  "C",      0.50,  false } });

    p->addSideBox ("FX MIX", "TAP", { "Post IR", "Pre IR" }, 0, "ACTIVE",
                   { "ROOM", "HALL", "DELAY", "MONO DLY", "DRY PAN" });
    return p;
}

std::unique_ptr<PagePanel> createRoutingPage()
{
    auto p = std::make_unique<PagePanel>();

    p->addCombo (PagePanel::row1 (0), "MODEL FOLDER", { "JK_clean" }, 0);
    p->addNote  ({ 34, 104, 300, 16 }, "OD_MIKE + 6 cab captures");
    p->addCatWatermark ({ 262, 100, 60, 34 });

    p->addKnobRow (PagePanel::row1 (1), "LEVELS",
                   { "IN", "OD", "AMP", "OUT" }, { 5.0, 5.5, 5.0, 5.0 });

    p->addBigKnob (PagePanel::row1 (2), "OUTPUT", 0.0, { -24.0, 24.0 },
                   [] (double v) { return juce::String (v, 1) + " dB"; });

    p->addWaveButton (PagePanel::row2 (0), "CAPTURE SET", "RELOAD");
    p->addSegmented  (PagePanel::row2 (1), "OD STAGE", { "ON", "BYPASS" }, 0);
    // no separate convolution stage any more - the cab lives in the capture
    p->addBlankCard  (PagePanel::row2 (2), "IR LOADER", "baked into capture");

    p->addFxSection ("SIGNAL",
                     { { "IN",     "0dB",   fxDots[0] },
                       { "GATE",   "-60dB", fxDots[1] },
                       { "OD",     "MIKE",  fxDots[2] },
                       { "AMP+IR", "IR 1",  fxDots[3] },
                       { "FX",     "79%",   fxDots[4] } },
                     { { "IN",     "0.0 dB",  0.50, false },
                       { "OD",     "ON",      0.55, true  },
                       { "AMP+IR", "IR 1",    1.00, true  },
                       { "FX",     "78.8 %",  0.788, false },
                       { "OUT",    "0.0 dB",  0.50, false } });

    p->addSideBox ("NAM MODELS", "RATE", { "48 kHz", "44.1 kHz" }, 0, "CHAIN",
                   { "OD_MIKE", "48kHz", "JK CLEAN", "IR 1", "READY" });
    return p;
}
