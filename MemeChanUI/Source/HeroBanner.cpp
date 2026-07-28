#include "HeroBanner.h"

using namespace theme;

HeroBanner::HeroBanner() {}

void HeroBanner::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    juce::Path clip;
    clip.addRoundedRectangle (b, 18.0f);
    g.reduceClipRegion (clip);

    g.setGradientFill ({ colours::heroBlue,     b.getCentreX(), b.getY(),
                         colours::heroBlueDeep, b.getCentreX(), b.getBottom(), false });
    g.fillRect (b);

    paintSpeedLines (g, b);

    auto burstArea = juce::Rectangle<float> (b.getWidth() * 0.60f, b.getHeight() * 0.86f)
                         .withCentre ({ b.getCentreX() + b.getWidth() * 0.09f,
                                        b.getCentreY() - b.getHeight() * 0.06f });

    paintBurst (g, burstArea);
    paintLogoText (g, burstArea);

    paintCat (g, juce::Rectangle<float> (b.getWidth() * 0.145f, b.getHeight() * 0.50f)
                     .withCentre ({ b.getX() + b.getWidth() * 0.235f,
                                    b.getCentreY() + b.getHeight() * 0.02f }));

    g.setColour (juce::Colour (0x14ffffff));
    g.drawRoundedRectangle (b.reduced (0.5f), 18.0f, 1.0f);
}

void HeroBanner::paintSpeedLines (juce::Graphics& g, juce::Rectangle<float> b) const
{
    const juce::Point<float> centre { b.getCentreX() + b.getWidth() * 0.08f,
                                      b.getCentreY() - b.getHeight() * 0.04f };
    const auto reach = b.getWidth();

    juce::Random rng (0xbeef42);
    juce::Path lines;

    for (int i = 0; i < 150; ++i)
    {
        const auto angle = juce::MathConstants<float>::twoPi * (float) i / 150.0f
                             + rng.nextFloat() * 0.02f;
        const auto inner = reach * (0.20f + rng.nextFloat() * 0.26f);
        const auto spread = 0.004f + rng.nextFloat() * 0.010f;

        const juce::Point<float> a { centre.x + std::sin (angle) * inner,
                                     centre.y - std::cos (angle) * inner };
        const juce::Point<float> c1 { centre.x + std::sin (angle - spread) * reach,
                                      centre.y - std::cos (angle - spread) * reach };
        const juce::Point<float> c2 { centre.x + std::sin (angle + spread) * reach,
                                      centre.y - std::cos (angle + spread) * reach };

        lines.startNewSubPath (a);
        lines.lineTo (c1);
        lines.lineTo (c2);
        lines.closeSubPath();
    }

    g.setColour (colours::heroStreak.withAlpha (0.55f));
    g.fillPath (lines);
}

void HeroBanner::paintBurst (juce::Graphics& g, juce::Rectangle<float> area) const
{
    const auto centre = area.getCentre();
    const auto rx = area.getWidth() * 0.5f;
    const auto ry = area.getHeight() * 0.5f;

    juce::Path burst;
    juce::Random rng (0x10fe);
    constexpr int spikes = 17;

    for (int i = 0; i < spikes * 2; ++i)
    {
        const auto a = juce::MathConstants<float>::twoPi * (float) i / (float) (spikes * 2);
        const auto k = (i % 2 == 0) ? 1.0f : 0.80f + rng.nextFloat() * 0.06f;
        const juce::Point<float> p { centre.x + std::sin (a) * rx * k,
                                     centre.y - std::cos (a) * ry * k };

        if (i == 0) burst.startNewSubPath (p);
        else        burst.lineTo (p);
    }

    burst.closeSubPath();

    juce::DropShadow (juce::Colour (0x3a1c2f5e), 22, { 0, 6 }).drawForPath (g, burst);

    g.setColour (colours::heroPaper);
    g.fillPath (burst);

    g.setColour (juce::Colour (0xff1d2430).withAlpha (0.85f));
    g.strokePath (burst, juce::PathStrokeType (2.2f, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    // halftone dots, bottom right
    g.setColour (colours::accent.withAlpha (0.55f));

    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 4; ++col)
            g.fillEllipse (centre.x + rx * 0.34f + (float) col * 11.0f,
                           centre.y + ry * 0.34f + (float) row * 11.0f,
                           4.5f, 4.5f);
}

void HeroBanner::paintLogoText (juce::Graphics& g, juce::Rectangle<float> area) const
{
    auto text = area.reduced (area.getWidth() * 0.14f, area.getHeight() * 0.18f);

    auto jpLine = text.removeFromTop (text.getHeight() * 0.40f);
    drawWeightedText (g, juce::String::fromUTF8 ("\xe3\x82\x81\xe3\x82\x81\xe3\x81\xa1\xe3\x82\x83\xe3\x82\x93"),
                      jp (jpLine.getHeight() * 0.92f), jpLine,
                      juce::Justification::centred, colours::accent, 4.0f, -0.06f);

    auto scream = text.removeFromTop (text.getHeight() * 0.58f);
    drawWeightedText (g, "Screaming!!", sans (scream.getHeight() * 0.92f, true, true), scream,
                      juce::Justification::centred, juce::Colour (0xff14181f), 3.0f, -0.16f);

    drawWeightedText (g, "DELUXE", sans (text.getHeight() * 0.86f, true, true), text,
                      juce::Justification::centred, colours::accent, 1.4f, -0.16f);

    // paw prints
    g.setColour (colours::accent.withAlpha (0.7f));

    for (int i = 0; i < 3; ++i)
    {
        const auto x = area.getRight() - area.getWidth() * 0.30f + (float) i * 22.0f;
        const auto y = area.getBottom() - area.getHeight() * 0.20f + (float) (i % 2) * 9.0f;

        g.fillEllipse (x, y + 5.0f, 11.0f, 8.5f);

        for (int t = 0; t < 3; ++t)
            g.fillEllipse (x + (float) t * 4.0f, y - 1.0f, 3.2f, 4.2f);
    }
}

void HeroBanner::paintCat (juce::Graphics& g, juce::Rectangle<float> area) const
{
    // rope, looping in from the top
    juce::Path rope;
    rope.startNewSubPath (area.getCentreX() - area.getWidth() * 0.10f, area.getY() - area.getHeight() * 0.52f);
    rope.quadraticTo (area.getCentreX() + area.getWidth() * 0.55f, area.getY() - area.getHeight() * 0.30f,
                      area.getCentreX() + area.getWidth() * 0.12f, area.getY() + area.getHeight() * 0.06f);

    g.setColour (juce::Colour (0xffe9dcc0));
    g.strokePath (rope, juce::PathStrokeType (13.0f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));
    g.setColour (juce::Colour (0xffc9b189));
    g.strokePath (rope, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));

    // cat, curled up
    auto body = area.withTrimmedTop (area.getHeight() * 0.10f);
    juce::Path shadow;
    shadow.addEllipse (body);
    juce::DropShadow (juce::Colour (0x55101a2e), 20, { 0, 8 }).drawForPath (g, shadow);

    drawCurledCat (g, body);

    // ears peeking over the top
    juce::Path ears;
    ears.startNewSubPath (body.getX() + body.getWidth() * 0.20f, body.getY() + body.getHeight() * 0.22f);
    ears.lineTo          (body.getX() + body.getWidth() * 0.13f, body.getY() - body.getHeight() * 0.06f);
    ears.lineTo          (body.getX() + body.getWidth() * 0.44f, body.getY() + body.getHeight() * 0.08f);
    ears.closeSubPath();

    ears.startNewSubPath (body.getRight() - body.getWidth() * 0.20f, body.getY() + body.getHeight() * 0.22f);
    ears.lineTo          (body.getRight() - body.getWidth() * 0.13f, body.getY() - body.getHeight() * 0.06f);
    ears.lineTo          (body.getRight() - body.getWidth() * 0.44f, body.getY() + body.getHeight() * 0.08f);
    ears.closeSubPath();

    g.setColour (juce::Colour (0xff191410));
    g.fillPath (ears);

    // motion ticks
    g.setColour (juce::Colour (0xfff2d98a));

    for (int i = 0; i < 3; ++i)
    {
        juce::Path tick;
        const auto x = area.getRight() + area.getWidth() * 0.22f + (float) i * 13.0f;
        const auto y = area.getY() - area.getHeight() * 0.30f + (float) i * 7.0f;
        tick.startNewSubPath (x, y);
        tick.lineTo (x + 9.0f, y - 15.0f);
        g.strokePath (tick, juce::PathStrokeType (3.4f, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));
    }
}
