#include "Widgets.h"

using namespace theme;

//==============================================================================
MemeChanLookAndFeel::MemeChanLookAndFeel()
{
    setColour (juce::ComboBox::backgroundColourId, colours::cardBg);
    setColour (juce::ComboBox::textColourId,       colours::textPrimary);
    setColour (juce::ComboBox::outlineColourId,    colours::cardBorder);
    setColour (juce::PopupMenu::backgroundColourId, colours::cardBg);
    setColour (juce::PopupMenu::textColourId,       colours::textPrimary);
}

void MemeChanLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                        int, int, int, int, juce::ComboBox& box)
{
    auto b = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height)
                 .reduced (strokeWidth() * 0.5f);

    const auto inked = currentSkin() == Skin::comic;
    auto p = roughRoundedRect (b, 9.0f, inked ? 1.1f : 0.0f, width * 3 + height);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillPath (p);

    strokeInked (g, p, box.findColour (juce::ComboBox::outlineColourId), strokeWidth(),
                 width * 17 + height);

    auto arrow = juce::Rectangle<float> (11.0f, 7.0f)
                     .withCentre ({ b.getRight() - 18.0f, b.getCentreY() });

    g.setColour (colours::textSecondary);
    g.fillPath (chevron (arrow, 0.0f));
}

juce::Font MemeChanLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return sans (14.0f);
}

void MemeChanLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& lbl)
{
    lbl.setBounds (14, 1, box.getWidth() - 40, box.getHeight() - 2);
    lbl.setFont (getComboBoxFont (box));
    lbl.setJustificationType (juce::Justification::centredLeft);
    lbl.setColour (juce::Label::textColourId, colours::textPrimary);
}

void MemeChanLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float, float,
                                            juce::Slider::SliderStyle, juce::Slider&)
{
    const auto cy = (float) y + (float) height * 0.5f;

    const auto inked = currentSkin() == Skin::comic;

    juce::Rectangle<float> track ((float) x, cy - 3.0f, (float) width, 6.0f);
    g.setColour (colours::meterTrack);
    g.fillRoundedRectangle (track, 3.0f);

    g.setColour (colours::accent);
    g.fillRoundedRectangle (track.withRight (sliderPos), 3.0f);

    if (inked)
    {
        g.setColour (colours::cardBorder);
        g.drawRoundedRectangle (track, 3.0f, 1.4f);
    }

    juce::Rectangle<float> thumb (17.0f, 17.0f);
    thumb.setCentre (sliderPos, cy);

    if (! inked)
    {
        juce::Path p;
        p.addEllipse (thumb);
        juce::DropShadow (juce::Colour (0x33000000), 5, { 0, 1 }).drawForPath (g, p);
    }

    g.setColour (inked ? colours::cardBg : juce::Colours::white);
    g.fillEllipse (thumb);
    g.setColour (inked ? colours::cardBorder : colours::textTertiary);
    g.drawEllipse (thumb.reduced (strokeWidth() * 0.5f), strokeWidth());
}

//==============================================================================
PawKnob::PawKnob()
{
    setSliderStyle (juce::Slider::RotaryVerticalDrag);
    setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                         juce::MathConstants<float>::pi * 2.75f, true);
}

void PawKnob::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    const auto d = juce::jmin (b.getWidth(), b.getHeight());
    auto area = juce::Rectangle<float> (d, d).withCentre (b.getCentre());

    const auto inked = currentSkin() == Skin::comic;

    if (inked)
    {
        g.setColour (colours::cardBorder.withAlpha (0.22f));
        g.fillEllipse (area.translated (3.0f, 3.5f));
    }
    else
    {
        juce::Path outline;
        outline.addEllipse (area.reduced (1.0f));
        juce::DropShadow (juce::Colour (0x26000000), 9, { 0, 3 }).drawForPath (g, outline);
    }

    // ivory metal rim
    g.setGradientFill ({ colours::knobRimHi, area.getCentreX(), area.getY(),
                         colours::knobRimLo, area.getCentreX(), area.getBottom(), false });
    g.fillEllipse (area);

    juce::Path rim;
    rim.addEllipse (area.reduced (strokeWidth() * 0.5f));
    strokeInked (g, rim, inked ? colours::cardBorder : juce::Colour (0xffc3bcac),
                 strokeWidth(), juce::roundToInt (area.getX() * 7.0f + d) + 5, 0.16f);

    // photographic face (placeholder artwork)
    auto face = area.reduced (d * 0.155f);
    drawCurledCat (g, face);

    // the face ring is short and tight, so it takes much less erosion
    juce::Path faceRing;
    faceRing.addEllipse (face);
    strokeInked (g, faceRing, inked ? colours::cardBorder : juce::Colour (0x40000000),
                 inked ? strokeWidth() : 1.0f,
                 juce::roundToInt (area.getY() * 9.0f + d) + 61, 0.10f);

    // position indicator
    const auto rp = getRotaryParameters();
    const auto prop = (float) valueToProportionOfLength (getValue());
    const auto angle = rp.startAngleRadians + prop * (rp.endAngleRadians - rp.startAngleRadians);
    const auto r = d * 0.5f - d * 0.075f;
    const juce::Point<float> dot { area.getCentreX() + std::sin (angle) * r,
                                   area.getCentreY() - std::cos (angle) * r };

    g.setColour (colours::accent);
    g.fillEllipse (juce::Rectangle<float> (d * 0.075f, d * 0.075f).withCentre (dot));
}

//==============================================================================
LabelledKnob::LabelledKnob (juce::String caption, juce::Range<double> range, double initialValue,
                            Formatter formatter, float knobDiameter)
    : captionText (std::move (caption)), format (std::move (formatter)), diameter (knobDiameter)
{
    knob.setRange (range.getStart(), range.getEnd(), 0.0);
    knob.setValue (initialValue, juce::dontSendNotification);
    addAndMakeVisible (knob);
}

void LabelledKnob::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    g.setColour (colours::textSecondary);
    g.setFont (label (11.0f));
    g.drawText (captionText, b.removeFromTop (16.0f), juce::Justification::centred);

    g.setColour (colours::textPrimary);
    g.setFont (sans (14.0f, true));
    g.drawText (format (knob.getValue()),
                b.removeFromBottom (18.0f), juce::Justification::centred);
}

void LabelledKnob::resized()
{
    auto b = getLocalBounds().reduced (0, 0);
    b.removeFromTop (20);
    b.removeFromBottom (20);
    knob.setBounds (juce::Rectangle<int> ((int) diameter, (int) diameter)
                        .withCentre (b.getCentre()));
}

//==============================================================================
FxKnobCell::FxKnobCell (juce::String name, juce::String valueText, double proportion)
    : nameText (std::move (name)), valueString (std::move (valueText))
{
    knob.setRange (0.0, 1.0, 0.0);
    knob.setValue (proportion, juce::dontSendNotification);
    addAndMakeVisible (knob);
}

void FxKnobCell::paint (juce::Graphics& g)
{
    auto textArea = getLocalBounds().toFloat().withTrimmedLeft (54.0f);

    g.setColour (colours::textPrimary);
    g.setFont (sans (13.0f, true));
    g.drawText (valueString, textArea.removeFromTop (textArea.getHeight() * 0.5f),
                juce::Justification::bottomLeft);

    g.setColour (colours::textSecondary);
    g.setFont (sans (11.5f));
    g.drawText (nameText, textArea, juce::Justification::topLeft);
}

void FxKnobCell::resized()
{
    knob.setBounds (0, (getHeight() - 48) / 2, 48, 48);
}

//==============================================================================
LevelMeter::LevelMeter (bool scaleOnRightSide) : scaleRight (scaleOnRightSide) {}

void LevelMeter::setLevelDb (float db)
{
    levelDb = db;
    repaint();
}

void LevelMeter::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    constexpr float barW = 9.0f;

    // leave room top and bottom so the end-stop labels are not clipped
    b.reduce (0.0f, 9.0f);

    auto bar   = scaleRight ? b.removeFromLeft (barW) : b.removeFromRight (barW);
    auto scale = b;

    g.setColour (colours::meterTrack);
    g.fillRoundedRectangle (bar, barW * 0.5f);

    if (currentSkin() == Skin::comic)
    {
        g.setColour (colours::cardBorder);
        g.drawRoundedRectangle (bar, barW * 0.5f, 1.4f);
    }

    const auto toY = [&bar] (float db)
    {
        const auto prop = juce::jlimit (0.0f, 1.0f, (db + 60.0f) / 72.0f);
        return bar.getBottom() - prop * bar.getHeight();
    };

    auto fill = bar.withTop (toY (levelDb));
    g.setGradientFill ({ colours::meterHi, bar.getCentreX(), bar.getBottom(),
                         colours::meterLo, bar.getCentreX(), bar.getY(), false });
    g.fillRoundedRectangle (fill, barW * 0.5f);

    const float ticks[] = { 12.0f, 0.0f, -12.0f, -24.0f, -36.0f, -60.0f };

    g.setFont (sans (10.0f));
    g.setColour (colours::textSecondary);

    for (auto db : ticks)
    {
        const auto y = toY (db);
        const auto text = db > 0.0f ? "+" + juce::String ((int) db) : juce::String ((int) db);

        g.drawText (text, scale.withY (y - 7.0f).withHeight (14.0f).reduced (4.0f, 0.0f),
                    scaleRight ? juce::Justification::centredLeft
                               : juce::Justification::centredRight);
    }
}

//==============================================================================
FlatButton::FlatButton (juce::String text, Style s, IconFn icon)
    : juce::Button (text), style (s), iconFn (std::move (icon))
{
    setButtonText (text);
}

void FlatButton::paintButton (juce::Graphics& g, bool highlighted, bool)
{
    auto b = getLocalBounds().toFloat().reduced (0.5f);
    const auto corner = juce::jmin (10.0f, b.getHeight() * 0.32f);

    juce::Colour bg, border, fg;

    switch (style)
    {
        case Style::plain:      bg = colours::cardBg;    border = colours::cardBorder; fg = colours::textPrimary;  break;
        case Style::outlined:   bg = colours::cardBg;    border = colours::accent;     fg = colours::accent;       break;
        case Style::filled:     bg = colours::accent;    border = colours::accent;     fg = juce::Colours::white;  break;
        case Style::ghost:      bg = {};                 border = {};                  fg = colours::textSecondary;break;
        case Style::accentText: bg = colours::cardBg;    border = colours::cardBorder; fg = colours::accent;       break;
    }

    const auto inked = currentSkin() == Skin::comic;
    b = b.reduced (inked ? strokeWidth() * 0.5f : 0.0f);

    auto p = roughRoundedRect (b, corner, inked ? 1.0f : 0.0f,
                               juce::roundToInt (b.getX() * 5.0f + b.getWidth() * 3.0f));

    if (! bg.isTransparent())
    {
        if (style != Style::ghost && ! inked)
            juce::DropShadow (juce::Colour (0x0d000000), 5, { 0, 1 }).drawForPath (g, p);

        g.setColour (highlighted ? bg.contrasting (0.04f) : bg);
        g.fillPath (p);
    }

    if (! border.isTransparent())
        strokeInked (g, p, border, strokeWidth(),
                     juce::roundToInt (b.getX() * 11.0f + b.getY() * 3.0f) + 41);

    const bool hasText = getButtonText().isNotEmpty();
    auto content = b.reduced (hasText ? 12.0f : 0.0f, 0.0f);

    if (iconFn)
    {
        const auto iconSize = juce::jmin (16.0f, b.getHeight() * 0.5f);
        auto iconArea = hasText ? content.removeFromLeft (iconSize + 7.0f)
                                         .withSizeKeepingCentre (iconSize, iconSize)
                                : b.withSizeKeepingCentre (iconSize, iconSize);
        g.setColour (style == Style::filled ? juce::Colours::white : iconColour);
        g.fillPath (iconFn (iconArea));
    }

    if (hasText)
    {
        g.setColour (fg);
        g.setFont (sans (fontHeight, true));
        g.drawText (getButtonText(), content,
                    iconFn ? juce::Justification::centredLeft : juce::Justification::centred);
    }
}

//==============================================================================
SegmentedControl::SegmentedControl (juce::StringArray options, int selected)
    : items (std::move (options)), selectedIndex (selected) {}

void SegmentedControl::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced (strokeWidth() * 0.5f);
    constexpr float corner = 9.0f;

    const auto inked = currentSkin() == Skin::comic;
    auto outer = roughRoundedRect (b, corner, inked ? 1.0f : 0.0f, getWidth() * 3 + getHeight());

    g.setColour (colours::cardBg);
    g.fillPath (outer);

    const auto segW = b.getWidth() / (float) items.size();

    for (int i = 0; i < items.size(); ++i)
    {
        auto seg = b.withWidth (segW).withX (b.getX() + segW * (float) i);
        const bool on = (i == selectedIndex);

        if (on)
        {
            g.setColour (colours::accent);
            g.fillRoundedRectangle (seg.reduced (inked ? 1.6f : 1.0f), corner - 1.0f);
        }

        g.setColour (on ? colours::cardBg : colours::textSecondary);
        g.setFont (sans (12.5f, true));
        g.drawText (items[i], seg, juce::Justification::centred);
    }

    strokeInked (g, outer, colours::cardBorder, strokeWidth(), getWidth() * 13 + 7);
}

void SegmentedControl::mouseDown (const juce::MouseEvent& e)
{
    const auto segW = (float) getWidth() / (float) items.size();
    selectedIndex = juce::jlimit (0, items.size() - 1, (int) ((float) e.x / segW));
    repaint();
}

//==============================================================================
TabStrip::TabStrip (std::vector<Tab> t, int selected)
    : tabs (std::move (t)), selectedIndex (selected) {}

juce::Rectangle<float> TabStrip::boundsForTab (int index) const
{
    const auto w = (float) getWidth() / (float) tabs.size();
    return { w * (float) index, 0.0f, w, (float) getHeight() };
}

void TabStrip::paint (juce::Graphics& g)
{
    const auto inked = currentSkin() == Skin::comic;

    if (! inked)
    {
        g.setColour (colours::cardBorder);
        g.fillRect (0.0f, (float) getHeight() - 1.0f, (float) getWidth(), 1.0f);
    }

    for (size_t i = 0; i < tabs.size(); ++i)
    {
        auto b = boundsForTab ((int) i);
        const bool on = ((int) i == selectedIndex);

        if (inked)
        {
            // separate inked plates; only the selected one runs into the panel
            auto plate = b.reduced (5.0f, 0.0f).withTrimmedTop (2.0f);
            juce::Path p;
            p.addRoundedRectangle (plate.getX(), plate.getY(), plate.getWidth(),
                                   plate.getHeight() + (on ? 14.0f : -6.0f), 11.0f, 11.0f,
                                   true, true, ! on, ! on);

            g.setColour (on ? colours::accent : colours::cardBg);
            g.fillPath (p);
            strokeInked (g, p, colours::cardBorder, strokeWidth(), (int) i * 137 + 23);

            if (on)
            {
                g.setColour (colours::cardBg);
                g.fillPath (boltIcon ({ plate.getRight() - 34.0f, plate.getCentreY() - 11.0f,
                                        15.0f, 22.0f }));
            }
        }
        else if (on)
        {
            juce::Path p;
            p.addRoundedRectangle (b.getX(), b.getY(), b.getWidth(), b.getHeight() + 12.0f,
                                   12.0f, 12.0f, true, true, false, false);
            g.setColour (colours::accent);
            g.fillPath (p);
        }

        const auto fg = on ? colours::cardBg
                           : (inked ? colours::textPrimary : colours::textSecondary);
        const auto textW = juce::GlyphArrangement::getStringWidth (sans (15.0f, true), tabs[i].name);
        const auto block = juce::Rectangle<float> (textW + 30.0f, b.getHeight())
                               .withCentre (b.getCentre());

        auto icons = block;
        auto iconArea = icons.removeFromLeft (22.0f).withSizeKeepingCentre (15.0f, 15.0f);

        if (tabs[i].icon)
        {
            g.setColour (fg);
            g.fillPath (tabs[i].icon (iconArea));
        }

        g.setColour (fg);
        g.setFont (sans (15.0f, true));
        g.drawText (tabs[i].name, icons, juce::Justification::centredLeft);
    }
}

void TabStrip::mouseDown (const juce::MouseEvent& e)
{
    const auto w = getWidth() / (int) tabs.size();
    setSelectedTab (e.x / juce::jmax (1, w));
}

void TabStrip::setSelectedTab (int index)
{
    index = juce::jlimit (0, (int) tabs.size() - 1, index);

    if (index == selectedIndex)
        return;

    selectedIndex = index;
    repaint();

    if (onTabChange != nullptr)
        onTabChange (selectedIndex);
}

//==============================================================================
FxChainStrip::FxChainStrip (std::vector<Item> i, bool compact)
    : items (std::move (i)), compactStyle (compact) {}

void FxChainStrip::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    const float arrowW  = compactStyle ? 11.0f : 26.0f;
    const float gapTotal = arrowW * (float) (items.size() - 1);
    const float pillW   = (b.getWidth() - gapTotal) / (float) items.size();
    const float nameSize = compactStyle ? 8.5f : 12.0f;

    float x = b.getX();

    for (size_t i = 0; i < items.size(); ++i)
    {
        juce::Rectangle<float> pill (x, b.getY(), pillW, b.getHeight());
        drawCard (g, pill.reduced (0.5f), compactStyle ? 8.0f : 10.0f);

        auto inner = pill.reduced (compactStyle ? 3.0f : 12.0f, 0.0f);

        if (! compactStyle)
        {
            auto dot = inner.removeFromLeft (18.0f).withSizeKeepingCentre (11.0f, 11.0f);
            g.setColour (items[i].dot);
            g.fillEllipse (dot);
            g.setColour (juce::Colours::white.withAlpha (0.8f));
            g.fillEllipse (dot.reduced (3.5f));

            g.setColour (colours::textSecondary);
            g.setFont (sans (11.5f));
            g.drawText (items[i].value, inner, juce::Justification::centredRight);
            inner.removeFromRight (juce::GlyphArrangement::getStringWidth (sans (11.5f), items[i].value) + 8.0f);
        }

        g.setColour (colours::textPrimary);
        g.setFont (sans (nameSize, ! compactStyle));
        g.drawText (items[i].name, inner,
                    compactStyle ? juce::Justification::centred : juce::Justification::centredLeft);

        x += pillW;

        if (i + 1 < items.size())
        {
            auto arrow = juce::Rectangle<float> (x, b.getY(), arrowW, b.getHeight());
            g.setColour (colours::textTertiary);
            g.fillPath (chevron (juce::Rectangle<float> (9.0f, 7.0f)
                                     .withCentre (arrow.getCentre()),
                                 -juce::MathConstants<float>::halfPi));
            x += arrowW;
        }
    }
}

//==============================================================================
WaveformView::WaveformView()
{
    juce::Random rng (0x51ee7);

    for (int i = 0; i < 220; ++i)
    {
        const auto env = 0.35f + 0.65f * std::sin ((float) i / 220.0f * 3.1f);
        samples.add (env * (0.25f + rng.nextFloat() * 0.75f));
    }
}

void WaveformView::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    const auto cy = b.getCentreY();
    const auto step = b.getWidth() / (float) samples.size();

    g.setColour (colours::accent.withAlpha (0.75f));

    for (int i = 0; i < samples.size(); ++i)
    {
        const auto h = samples[i] * b.getHeight() * 0.46f;
        g.fillRect (b.getX() + (float) i * step, cy - h, juce::jmax (1.0f, step * 0.55f), h * 2.0f);
    }
}
