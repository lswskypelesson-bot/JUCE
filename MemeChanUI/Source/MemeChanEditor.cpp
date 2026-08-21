#include "MemeChanEditor.h"

using namespace theme;

//==============================================================================
namespace
{
    juce::String oneDecimal (double v) { return juce::String (v, 1); }

    //--------------------------------------------------------------------------
    juce::Path ampTabIcon (juce::Rectangle<float> a)
    {
        juce::Path p, box;
        box.addRoundedRectangle (a, a.getWidth() * 0.16f);
        juce::PathStrokeType (a.getWidth() * 0.11f).createStrokedPath (p, box);

        p.addEllipse (a.reduced (a.getWidth() * 0.30f));
        p.addRectangle (a.getX() + a.getWidth() * 0.18f, a.getY() + a.getHeight() * 0.16f,
                        a.getWidth() * 0.64f, a.getHeight() * 0.06f);
        return p;
    }

    juce::Path driveTabIcon (juce::Rectangle<float> a)
    {
        juce::Path p;
        const auto c = a.getCentre();
        const auto r = a.getWidth() * 0.5f;

        juce::Path ring;
        ring.addEllipse (a.reduced (r * 0.55f));
        juce::PathStrokeType (r * 0.22f).createStrokedPath (p, ring);

        for (int i = 0; i < 8; ++i)
        {
            const auto ang = juce::MathConstants<float>::twoPi * (float) i / 8.0f;
            juce::Path ray;
            ray.startNewSubPath (c.x + std::sin (ang) * r * 0.62f, c.y - std::cos (ang) * r * 0.62f);
            ray.lineTo          (c.x + std::sin (ang) * r * 0.98f, c.y - std::cos (ang) * r * 0.98f);
            juce::PathStrokeType (r * 0.20f, juce::PathStrokeType::curved,
                                  juce::PathStrokeType::rounded).createStrokedPath (ray, ray);
            p.addPath (ray);
        }

        return p;
    }

    juce::Path fxTabIcon (juce::Rectangle<float> a)
    {
        juce::Path p = sparkle (a.getCentre().translated (a.getWidth() * 0.08f, a.getHeight() * 0.08f),
                                a.getWidth() * 0.42f);
        p.addPath (sparkle (a.getTopLeft().translated (a.getWidth() * 0.16f, a.getHeight() * 0.14f),
                            a.getWidth() * 0.18f));
        return p;
    }

    juce::Path routingTabIcon (juce::Rectangle<float> a)
    {
        juce::Path p, left, right;
        left.addEllipse  (a.getX(), a.getY() + a.getHeight() * 0.18f,
                          a.getWidth() * 0.58f, a.getHeight() * 0.64f);
        right.addEllipse (a.getX() + a.getWidth() * 0.42f, a.getY() + a.getHeight() * 0.18f,
                          a.getWidth() * 0.58f, a.getHeight() * 0.64f);
        juce::PathStrokeType (a.getWidth() * 0.11f).createStrokedPath (p, left);

        juce::Path r2;
        juce::PathStrokeType (a.getWidth() * 0.11f).createStrokedPath (r2, right);
        p.addPath (r2);
        return p;
    }

    //--------------------------------------------------------------------------
    /** Preset browser: prev / name / dropdown, all in one rounded field. */
    class PresetBar final : public juce::Component
    {
    public:
        void paint (juce::Graphics& g) override
        {
            auto b = getLocalBounds().toFloat();
            drawCard (g, b.reduced (0.5f), 11.0f);

            g.setColour (colours::textSecondary);

            auto left = juce::Rectangle<float> (12.0f, 9.0f)
                            .withCentre ({ b.getX() + 26.0f, b.getCentreY() });
            g.fillPath (chevron (left, juce::MathConstants<float>::halfPi));

            auto right = juce::Rectangle<float> (12.0f, 9.0f)
                             .withCentre ({ b.getRight() - 26.0f, b.getCentreY() });
            g.fillPath (chevron (right, 0.0f));

            g.setColour (colours::textPrimary);
            g.setFont (sans (15.0f));
            g.drawText ("Init", b.withTrimmedLeft (48.0f).withTrimmedRight (44.0f),
                        juce::Justification::centredLeft);
        }
    };

    //--------------------------------------------------------------------------
    /** INPUT / OUTPUT trim: caption, meter, knob, read-out. */
    class IOCard final : public juce::Component
    {
    public:
        IOCard (juce::String caption, bool meterOnLeft)
            : title (std::move (caption)), meterLeft (meterOnLeft), meter (meterOnLeft)
        {
            knob.setRange (-24.0, 24.0, 0.0);
            knob.setValue (0.0, juce::dontSendNotification);
            meter.setLevelDb (meterOnLeft ? -20.0f : -17.0f);

            addAndMakeVisible (meter);
            addAndMakeVisible (knob);
        }

        void paint (juce::Graphics& g) override
        {
            auto b = getLocalBounds().toFloat();
            drawCard (g, b.reduced (0.5f), 14.0f);

            g.setColour (colours::textSecondary);
            g.setFont (label (11.5f));
            g.drawText (title, b.withHeight (34.0f), juce::Justification::centred);

            g.setColour (colours::textPrimary);
            g.setFont (sans (13.5f, true));
            g.drawText (juce::String (knob.getValue(), 1) + " dB",
                        juce::Rectangle<float> (knob.getX() - 12.0f, 286.0f,
                                                (float) knob.getWidth() + 24.0f, 20.0f),
                        juce::Justification::centred);
        }

        void resized() override
        {
            meter.setBounds (meterLeft ? juce::Rectangle<int> (14, 44, 64, 236)
                                       : juce::Rectangle<int> (90, 44, 64, 236));
            knob .setBounds (meterLeft ? juce::Rectangle<int> (56, 178, 96, 96)
                                       : juce::Rectangle<int> (16, 178, 96, 96));
        }

    private:
        juce::String title;
        bool meterLeft;
        LevelMeter meter;
        PawKnob knob;
    };

    //--------------------------------------------------------------------------
    /** GAIN / GT'S TONE / LEVEL, floating over the bottom of the banner. */
    class HeroKnobPanel final : public juce::Component
    {
    public:
        HeroKnobPanel()
        {
            for (auto* n : { "GAIN", "GT'S TONE", "LEVEL" })
            {
                auto k = std::make_unique<LabelledKnob> (n, juce::Range<double> (0.0, 10.0),
                                                         5.0, oneDecimal, 84.0f);
                addAndMakeVisible (*k);
                knobs.push_back (std::move (k));
            }
        }

        void paint (juce::Graphics& g) override
        {
            drawCard (g, getLocalBounds().toFloat().reduced (0.5f), 18.0f);
        }

        void resized() override
        {
            for (size_t i = 0; i < knobs.size(); ++i)
                knobs[i]->setBounds (16 + (int) i * 209, 16, 200, 156);
        }

    private:
        std::vector<std::unique_ptr<LabelledKnob>> knobs;
    };
}

//==============================================================================
MemeChanEditor::MemeChanEditor()
{
    setLookAndFeel (&lnf);
    setOpaque (true);

    addAndMakeVisible (hero);

    presetBar = std::make_unique<PresetBar>();
    addAndMakeVisible (*presetBar);

    for (auto* b : { &saveButton, &saveAsButton, &moreButton, &inputButton,
                     &filterButton, &gearButton })
        addAndMakeVisible (*b);

    inputButton .setIconColour (colours::accent);
    filterButton.setIconColour (colours::textSecondary);
    gearButton  .setIconColour (colours::textSecondary);

    // parked for now, along with the EQ and pickup sections
    filterButton.setEnabled (false);

    inputCard     = std::make_unique<IOCard> ("INPUT",  true);
    outputCard    = std::make_unique<IOCard> ("OUTPUT", false);
    heroKnobPanel = std::make_unique<HeroKnobPanel>();

    addAndMakeVisible (*inputCard);
    addAndMakeVisible (*outputCard);
    addAndMakeVisible (*heroKnobPanel);

    tabs = std::make_unique<TabStrip> (std::vector<TabStrip::Tab> {
                                           { "AMP",     ampTabIcon },
                                           { "DRIVE",   driveTabIcon },
                                           { "FX",      fxTabIcon },
                                           { "ROUTING", routingTabIcon } },
                                       0);
    tabs->onTabChange = [this] (int i) { showPage (i); };
    addAndMakeVisible (*tabs);

    pages.push_back (createAmpPage());
    pages.push_back (createDrivePage());
    pages.push_back (createFxPage());
    pages.push_back (createRoutingPage());

    for (auto& p : pages)
        addChildComponent (*p);

    showPage (0);

    oversamplingBox.addItemList ({ "Off", "Low", "High", "Ultra" }, 1);
    oversamplingBox.setSelectedId (3, juce::dontSendNotification);
    addAndMakeVisible (oversamplingBox);

    presetInitBox.addItemList ({ "Init", "Factory" }, 1);
    presetInitBox.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (presetInitBox);

    setSize (designWidth, designHeight);
}

MemeChanEditor::~MemeChanEditor()
{
    setLookAndFeel (nullptr);
}

void MemeChanEditor::showPage (int index)
{
    for (size_t i = 0; i < pages.size(); ++i)
        pages[i]->setVisible ((int) i == index);

    if (tabs != nullptr)
        tabs->setSelectedTab (index);
}

//==============================================================================
void MemeChanEditor::resized()
{
    scale = (float) getWidth() / (float) designWidth;

    auto R = [this] (int x, int y, int w, int h)
    {
        return juce::Rectangle<int> (juce::roundToInt ((float) x * scale),
                                     juce::roundToInt ((float) y * scale),
                                     juce::roundToInt ((float) w * scale),
                                     juce::roundToInt ((float) h * scale));
    };

    presetBar   ->setBounds (R (560,  18, 296, 52));
    saveButton   .setBounds (R (878,  22,  82, 44));
    saveAsButton .setBounds (R (970,  22,  94, 44));
    moreButton   .setBounds (R (1074, 22,  54, 44));
    inputButton  .setBounds (R (1174, 22,  98, 44));
    filterButton .setBounds (R (1282, 22,  92, 44));
    gearButton   .setBounds (R (1384, 22,  44, 44));

    hero          .setBounds (R (22,   84, 1404, 436));
    inputCard    ->setBounds (R (32,  128,  168, 344));
    outputCard   ->setBounds (R (1248, 128, 168, 344));
    heroKnobPanel->setBounds (R (398, 388,  656, 188));

    tabs->setBounds (R (22, 590, 1404, 54));

    // Pages lay themselves out in design units; the transform handles the
    // window scale so page code never has to think about it.
    for (auto& p : pages)
    {
        p->setBounds (mainPanelBounds);
        p->setTransform (juce::AffineTransform::scale (scale));
    }

    oversamplingBox.setBounds (R (146, 1042, 116, 30));
    presetInitBox  .setBounds (R (372, 1042,  92, 30));
}

//==============================================================================
void MemeChanEditor::paint (juce::Graphics& g)
{
    g.fillAll (colours::windowBg);

    juce::Graphics::ScopedSaveState save (g);
    g.addTransform (juce::AffineTransform::scale (scale));

    paintTopBar (g);
    drawCard (g, mainPanelBounds.toFloat(), 16.0f);
    paintBottomBar (g);
}

void MemeChanEditor::paintOverChildren (juce::Graphics& g)
{
    // paper stock sits on top of everything, as it would in print
    juce::Graphics::ScopedSaveState save (g);
    g.addTransform (juce::AffineTransform::scale (scale));

    const juce::Rectangle<float> full (0.0f, 0.0f, (float) designWidth, (float) designHeight);
    paperGrain (g, full);

    if (currentSkin() == Skin::comic)
    {
        juce::Path frame;
        frame.addRectangle (full.reduced (1.5f));
        strokeInked (g, frame, colours::cardBorder, 3.0f, 20260728);
    }
}

void MemeChanEditor::paintTopBar (juce::Graphics& g)
{
    g.setColour (colours::cardBg);
    g.fillRect (0, 0, designWidth, 88);

    if (currentSkin() == Skin::comic)
    {
        g.setColour (colours::cardBorder);
        g.fillRect (0.0f, 86.0f, (float) designWidth, 2.0f);
    }

    drawCat (g, { 24.0f, 24.0f, 38.0f, 38.0f }, colours::catBlack);

    drawWeightedText (g, "MemeChan Screaming!!", sans (22.0f, true, true),
                      { 76.0f, 26.0f, 264.0f, 34.0f }, juce::Justification::centredLeft,
                      colours::textPrimary, 0.4f);

    drawWeightedText (g, "DELUXE", sans (20.0f, true, true),
                      { 352.0f, 27.0f, 100.0f, 32.0f }, juce::Justification::centredLeft,
                      colours::accent, 0.4f);

    g.setColour (colours::accent);
    g.fillPath (sparkle ({ 472.0f, 40.0f }, 9.5f));
    g.fillPath (sparkle ({ 486.0f, 28.0f }, 4.5f));
}

void MemeChanEditor::paintBottomBar (juce::Graphics& g)
{
    g.setColour (colours::cardBg);
    g.fillRect (0, 1028, designWidth, designHeight - 1028);

    g.setColour (colours::cardBorder);
    g.fillRect (0.0f, 1028.0f, (float) designWidth, strokeWidth());

    g.setColour (colours::textSecondary);
    g.setFont (label (10.5f));
    g.drawText ("OVERSAMPLING", 30, 1028, 112, 58, juce::Justification::centredLeft);
    g.drawText ("PRESET INIT",  276, 1028,  92, 58, juce::Justification::centredLeft);

    g.fillPath (headphoneIcon ({ 986.0f, 1046.0f, 22.0f, 22.0f }));
    g.fillPath (smileyIcon    ({ 1042.0f, 1046.0f, 21.0f, 21.0f }));

    g.setFont (sans (12.5f));
    g.setColour (colours::textPrimary);
    g.drawText ("MemeChan, Inc.", 1076, 1028, 140, 58, juce::Justification::centredLeft);

    g.setColour (colours::textSecondary);
    g.drawText ("v1.0.0", 1274, 1028, 70, 58, juce::Justification::centredLeft);

    drawCat (g, { 1396.0f, 1042.0f, 28.0f, 28.0f }, colours::catBlack);
}
