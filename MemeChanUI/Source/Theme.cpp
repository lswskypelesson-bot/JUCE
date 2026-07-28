#include "Theme.h"

namespace theme
{
    static juce::Font makeFont (const juce::String& family, float height, const juce::String& style)
    {
        auto opts = juce::FontOptions{}.withName (family).withHeight (height);

        if (style.isNotEmpty())
            opts = opts.withStyle (style);

        return juce::Font (opts);
    }

    juce::Font sans (float height, bool bold, bool italic)
    {
        juce::String style;

        if (bold && italic)  style = "Bold Italic";
        else if (bold)       style = "Bold";
        else if (italic)     style = "Italic";

        return makeFont ("Liberation Sans", height, style);
    }

    juce::Font jp (float height)
    {
        return makeFont ("IPAGothic", height, {});
    }

    juce::Font label (float height)
    {
        return sans (height, true);
    }

    void drawCard (juce::Graphics& g, juce::Rectangle<float> bounds, float corner,
                   juce::Colour fill, juce::Colour border)
    {
        juce::Path p;
        p.addRoundedRectangle (bounds, corner);

        juce::DropShadow (juce::Colour (0x14000000), 10, { 0, 2 }).drawForPath (g, p);

        g.setColour (fill);
        g.fillPath (p);

        g.setColour (border);
        g.strokePath (p, juce::PathStrokeType (1.0f));
    }

    void drawWeightedText (juce::Graphics& g, const juce::String& text, const juce::Font& font,
                           juce::Rectangle<float> area, juce::Justification just,
                           juce::Colour fill, float extraWeight, float shear)
    {
        juce::GlyphArrangement ga;
        ga.addFittedText (font, text, area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                          just, 1, 1.0f);

        juce::Path p;
        ga.createPath (p);

        if (shear != 0.0f)
        {
            auto c = p.getBounds().getCentre();
            p.applyTransform (juce::AffineTransform::shear (shear, 0.0f)
                                  .translated (-shear * c.y, 0.0f));
        }

        g.setColour (fill);

        if (extraWeight > 0.0f)
            g.strokePath (p, juce::PathStrokeType (extraWeight, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

        g.fillPath (p);
    }

    void drawCat (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour colour)
    {
        const auto w = area.getWidth();
        const auto h = area.getHeight();
        const auto x = area.getX();
        const auto y = area.getY();

        juce::Path p;

        // ears
        p.startNewSubPath (x + w * 0.14f, y + h * 0.44f);
        p.lineTo          (x + w * 0.10f, y + h * 0.02f);
        p.lineTo          (x + w * 0.42f, y + h * 0.20f);
        p.closeSubPath();

        p.startNewSubPath (x + w * 0.86f, y + h * 0.44f);
        p.lineTo          (x + w * 0.90f, y + h * 0.02f);
        p.lineTo          (x + w * 0.58f, y + h * 0.20f);
        p.closeSubPath();

        // head
        p.addEllipse (x + w * 0.06f, y + h * 0.18f, w * 0.88f, h * 0.78f);

        g.setColour (colour);
        g.fillPath (p);

        // eyes - tied to the body alpha so the mark can be used as a watermark
        g.setColour (juce::Colours::white.withAlpha (colour.getFloatAlpha() * 0.92f));
        g.fillEllipse (x + w * 0.28f, y + h * 0.48f, w * 0.11f, h * 0.15f);
        g.fillEllipse (x + w * 0.61f, y + h * 0.48f, w * 0.11f, h * 0.15f);
    }

    void drawCurledCat (juce::Graphics& g, juce::Rectangle<float> area)
    {
        // Stand-in for the photographic cut-out: a dark curled shape with a
        // warm rim light, so the knob faces read the way the artwork will.
        auto c = area.getCentre();

        juce::ColourGradient grad (juce::Colour (0xff5a4638),
                                   c.x - area.getWidth() * 0.22f,
                                   c.y - area.getHeight() * 0.28f,
                                   juce::Colour (0xff090707),
                                   c.x + area.getWidth() * 0.32f,
                                   c.y + area.getHeight() * 0.40f,
                                   true);
        grad.addColour (0.45, juce::Colour (0xff231a15));

        g.setGradientFill (grad);
        g.fillEllipse (area);

        // tail curling round the bottom
        juce::Path tail;
        tail.startNewSubPath (area.getX() + area.getWidth() * 0.20f,
                              area.getY() + area.getHeight() * 0.72f);
        tail.quadraticTo (area.getCentreX(), area.getBottom() + area.getHeight() * 0.10f,
                          area.getRight() - area.getWidth() * 0.14f,
                          area.getY() + area.getHeight() * 0.60f);

        g.setColour (juce::Colour (0xff241b15).withAlpha (0.55f));
        g.strokePath (tail, juce::PathStrokeType (area.getWidth() * 0.10f,
                                                  juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

        // soft highlight
        g.setColour (juce::Colours::white.withAlpha (0.06f));
        g.fillEllipse (area.getX() + area.getWidth() * 0.18f,
                       area.getY() + area.getHeight() * 0.10f,
                       area.getWidth() * 0.34f, area.getHeight() * 0.22f);
    }

    juce::Path chevron (juce::Rectangle<float> area, float rotationRadians)
    {
        juce::Path p;
        p.startNewSubPath (-0.5f,  -0.28f);
        p.lineTo          ( 0.0f,   0.22f);
        p.lineTo          ( 0.5f,  -0.28f);

        auto stroked = juce::Path();
        juce::PathStrokeType (0.14f, juce::PathStrokeType::curved,
                              juce::PathStrokeType::rounded).createStrokedPath (stroked, p);

        stroked.applyTransform (juce::AffineTransform::rotation (rotationRadians)
                                    .scaled (area.getWidth(), area.getHeight())
                                    .translated (area.getCentreX(), area.getCentreY()));
        return stroked;
    }

    juce::Path gearIcon (juce::Rectangle<float> area)
    {
        juce::Path p;
        const auto c = area.getCentre();
        const auto rOuter = area.getWidth() * 0.5f;
        const auto rInner = rOuter * 0.72f;
        const int teeth = 8;

        for (int i = 0; i < teeth * 2; ++i)
        {
            const auto a = juce::MathConstants<float>::twoPi * (float) i / (float) (teeth * 2);
            const auto r = (i % 2 == 0) ? rOuter : rInner;
            const juce::Point<float> pt { c.x + std::sin (a) * r, c.y - std::cos (a) * r };

            if (i == 0) p.startNewSubPath (pt);
            else        p.lineTo (pt);
        }

        p.closeSubPath();
        p.addEllipse (c.x - rOuter * 0.32f, c.y - rOuter * 0.32f, rOuter * 0.64f, rOuter * 0.64f);
        p.setUsingNonZeroWinding (false);
        return p;
    }

    juce::Path funnelIcon (juce::Rectangle<float> area)
    {
        juce::Path p;
        const auto x = area.getX(), y = area.getY(), w = area.getWidth(), h = area.getHeight();

        p.startNewSubPath (x,           y + h * 0.10f);
        p.lineTo          (x + w,       y + h * 0.10f);
        p.lineTo          (x + w * 0.60f, y + h * 0.52f);
        p.lineTo          (x + w * 0.60f, y + h * 0.98f);
        p.lineTo          (x + w * 0.40f, y + h * 0.80f);
        p.lineTo          (x + w * 0.40f, y + h * 0.52f);
        p.closeSubPath();
        return p;
    }

    juce::Path waveIcon (juce::Rectangle<float> area)
    {
        juce::Path p;
        const float heights[] = { 0.34f, 0.72f, 1.0f, 0.56f, 0.86f, 0.30f };
        const int n = juce::numElementsInArray (heights);
        const auto barW = area.getWidth() / (float) (n * 2 - 1);

        for (int i = 0; i < n; ++i)
        {
            const auto hgt = area.getHeight() * heights[i];
            p.addRoundedRectangle (area.getX() + (float) i * barW * 2.0f,
                                   area.getCentreY() - hgt * 0.5f,
                                   barW, hgt, barW * 0.5f);
        }

        return p;
    }

    juce::Path headphoneIcon (juce::Rectangle<float> area)
    {
        juce::Path p;
        const auto x = area.getX(), y = area.getY(), w = area.getWidth(), h = area.getHeight();

        juce::Path band;
        band.addCentredArc (x + w * 0.5f, y + h * 0.55f, w * 0.42f, h * 0.42f, 0.0f,
                            -juce::MathConstants<float>::halfPi * 1.9f,
                             juce::MathConstants<float>::halfPi * 1.9f, true);
        juce::PathStrokeType (w * 0.12f, juce::PathStrokeType::curved,
                              juce::PathStrokeType::rounded).createStrokedPath (p, band);

        p.addRoundedRectangle (x + w * 0.02f, y + h * 0.50f, w * 0.22f, h * 0.40f, w * 0.08f);
        p.addRoundedRectangle (x + w * 0.76f, y + h * 0.50f, w * 0.22f, h * 0.40f, w * 0.08f);
        return p;
    }

    juce::Path smileyIcon (juce::Rectangle<float> area)
    {
        juce::Path p;
        juce::Path ring;
        ring.addEllipse (area);
        juce::PathStrokeType (area.getWidth() * 0.09f).createStrokedPath (p, ring);

        const auto c = area.getCentre();
        const auto r = area.getWidth() * 0.5f;
        p.addEllipse (c.x - r * 0.42f, c.y - r * 0.30f, r * 0.17f, r * 0.24f);
        p.addEllipse (c.x + r * 0.25f, c.y - r * 0.30f, r * 0.17f, r * 0.24f);

        juce::Path mouth;
        mouth.addCentredArc (c.x, c.y, r * 0.52f, r * 0.52f, 0.0f,
                             juce::MathConstants<float>::halfPi * 0.6f,
                             juce::MathConstants<float>::pi * 0.86f, true);
        juce::PathStrokeType (area.getWidth() * 0.09f, juce::PathStrokeType::curved,
                              juce::PathStrokeType::rounded).createStrokedPath (mouth, mouth);
        p.addPath (mouth);
        return p;
    }

    juce::Path sparkle (juce::Point<float> centre, float radius)
    {
        juce::Path p;
        const float k = radius * 0.28f;

        p.startNewSubPath (centre.x,            centre.y - radius);
        p.quadraticTo     (centre.x + k,        centre.y - k,        centre.x + radius, centre.y);
        p.quadraticTo     (centre.x + k,        centre.y + k,        centre.x,          centre.y + radius);
        p.quadraticTo     (centre.x - k,        centre.y + k,        centre.x - radius, centre.y);
        p.quadraticTo     (centre.x - k,        centre.y - k,        centre.x,          centre.y - radius);
        p.closeSubPath();
        return p;
    }
}
