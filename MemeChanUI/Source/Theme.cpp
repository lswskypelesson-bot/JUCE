#include "Theme.h"

namespace theme
{
    namespace colours
    {
        juce::Colour windowBg, cardBg, cardBorder, panelBg;
        juce::Colour textPrimary, textSecondary, textTertiary;
        juce::Colour accent, accentHover, accentSoft;
        juce::Colour heroBlue, heroBlueDeep, heroStreak, heroPaper;
        juce::Colour knobRimHi, knobRimLo, knobFaceHi, knobFaceLo;
        juce::Colour meterTrack, meterLo, meterHi;
        juce::Colour catBlack;
    }

    static Skin activeSkin = Skin::modern;

    Skin currentSkin() noexcept { return activeSkin; }
    float strokeWidth() noexcept { return activeSkin == Skin::comic ? 2.2f : 1.0f; }

    void setSkin (Skin s)
    {
        using namespace colours;
        activeSkin = s;

        if (s == Skin::modern)
        {
            windowBg     = juce::Colour (0xfff5f5f7);
            cardBg       = juce::Colour (0xffffffff);
            cardBorder   = juce::Colour (0xffe4e6ea);
            panelBg      = juce::Colour (0xfffbfbfc);

            textPrimary  = juce::Colour (0xff191c22);
            textSecondary= juce::Colour (0xff8f959f);
            textTertiary = juce::Colour (0xffb6bbc4);

            accent       = juce::Colour (0xff3b5fe0);
            accentHover  = juce::Colour (0xff2f4fc7);
            accentSoft   = juce::Colour (0xffe9effc);

            heroBlue     = juce::Colour (0xff5385d8);
            heroBlueDeep = juce::Colour (0xff3f6cc4);
            heroStreak   = juce::Colour (0xffc9dcf5);
            heroPaper    = juce::Colour (0xfffcfaf4);

            knobRimHi    = juce::Colour (0xfffefdfa);
            knobRimLo    = juce::Colour (0xffd4cec1);
            knobFaceHi   = juce::Colour (0xff4a3b31);
            knobFaceLo   = juce::Colour (0xff0d0a09);

            meterTrack   = juce::Colour (0xffe8eaee);
            meterLo      = juce::Colour (0xff6f8ef0);
            meterHi      = juce::Colour (0xff3b5fe0);

            catBlack     = juce::Colour (0xff17181c);
        }
        else
        {
            // riso / comic print: cream stock, black ink, one flat ultramarine
            windowBg     = juce::Colour (0xffece5d5);
            cardBg       = juce::Colour (0xfff6f1e2);
            cardBorder   = juce::Colour (0xff17130f);
            panelBg      = juce::Colour (0xfff1ebda);

            textPrimary  = juce::Colour (0xff17130f);
            textSecondary= juce::Colour (0xff6d6455);
            textTertiary = juce::Colour (0xff9a9284);

            accent       = juce::Colour (0xff1e2bc8);
            accentHover  = juce::Colour (0xff1721a0);
            accentSoft   = juce::Colour (0xffc6c9ee);

            heroBlue     = juce::Colour (0xff1f2ac4);
            heroBlueDeep = juce::Colour (0xff141c94);
            heroStreak   = juce::Colour (0xff0b0f5c);
            heroPaper    = juce::Colour (0xfff4eedd);

            knobRimHi    = juce::Colour (0xfff6f1e2);
            knobRimLo    = juce::Colour (0xffcfc6ad);
            knobFaceHi   = juce::Colour (0xff43352b);
            knobFaceLo   = juce::Colour (0xff0b0908);

            meterTrack   = juce::Colour (0xffdcd4c0);
            meterLo      = juce::Colour (0xff4a55dd);
            meterHi      = juce::Colour (0xff1e2bc8);

            catBlack     = juce::Colour (0xff17130f);
        }
    }

    struct SkinInit { SkinInit() { setSkin (Skin::modern); } };
    static SkinInit skinInit;

    //==========================================================================
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

    juce::Font jp (float height)   { return makeFont ("IPAGothic", height, {}); }
    juce::Font label (float height) { return sans (height, true); }

    //==========================================================================
    juce::Path roughRoundedRect (juce::Rectangle<float> r, float corner,
                                 float jitter, int seed)
    {
        juce::Path base;
        base.addRoundedRectangle (r, corner);

        if (jitter <= 0.0f)
            return base;

        juce::Path out;
        juce::Random rng (seed);
        juce::PathFlatteningIterator it (base, {}, 4.0f);
        bool first = true;

        while (it.next())
        {
            const auto jx = (rng.nextFloat() - 0.5f) * jitter;
            const auto jy = (rng.nextFloat() - 0.5f) * jitter;

            if (first)
            {
                out.startNewSubPath (it.x1 + jx, it.y1 + jy);
                first = false;
            }

            out.lineTo (it.x2 + jx, it.y2 + jy);
        }

        out.closeSubPath();
        return out;
    }

    void halftone (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour colour,
                   float spacing, float radius, float angleRadians)
    {
        juce::Graphics::ScopedSaveState save (g);
        g.reduceClipRegion (area.getSmallestIntegerContainer());
        g.setColour (colour);

        const auto diag = std::hypot (area.getWidth(), area.getHeight());
        const auto centre = area.getCentre();
        const auto cosA = std::cos (angleRadians), sinA = std::sin (angleRadians);

        for (float v = -diag * 0.5f; v < diag * 0.5f; v += spacing)
            for (float u = -diag * 0.5f; u < diag * 0.5f; u += spacing)
            {
                const auto x = centre.x + u * cosA - v * sinA;
                const auto y = centre.y + u * sinA + v * cosA;
                g.fillEllipse (x - radius, y - radius, radius * 2.0f, radius * 2.0f);
            }
    }

    /** 128x128 tile of speckle, built once. */
    static const juce::Image& grainTile()
    {
        static juce::Image tile = []
        {
            juce::Image img (juce::Image::ARGB, 128, 128, true);
            juce::Random rng (0x9a17e);

            for (int y = 0; y < img.getHeight(); ++y)
                for (int x = 0; x < img.getWidth(); ++x)
                {
                    const auto n = rng.nextFloat();

                    if (n > 0.72f)
                        img.setPixelAt (x, y, juce::Colour (0xff2b241a)
                                                  .withAlpha ((n - 0.72f) * 0.42f));
                }

            return img;
        }();

        return tile;
    }

    void paperGrain (juce::Graphics& g, juce::Rectangle<float> area)
    {
        if (activeSkin != Skin::comic)
            return;

        juce::Graphics::ScopedSaveState save (g);
        g.setTiledImageFill (grainTile(), 0, 0, 0.85f);
        g.fillRect (area);
    }

    //==========================================================================
    void drawCard (juce::Graphics& g, juce::Rectangle<float> bounds, float corner,
                   juce::Colour fill, juce::Colour border)
    {
        if (fill  == juce::Colour()) fill  = colours::cardBg;
        if (border == juce::Colour()) border = colours::cardBorder;

        if (activeSkin == Skin::modern)
        {
            juce::Path p;
            p.addRoundedRectangle (bounds, corner);

            juce::DropShadow (juce::Colour (0x14000000), 10, { 0, 2 }).drawForPath (g, p);

            g.setColour (fill);
            g.fillPath (p);

            g.setColour (border);
            g.strokePath (p, juce::PathStrokeType (1.0f));
            return;
        }

        // comic: an offset ink plate behind, then the panel, then the outline
        const auto seed = juce::roundToInt (bounds.getX() * 7.0f + bounds.getY() * 13.0f
                                            + bounds.getWidth());
        auto p = roughRoundedRect (bounds, corner, 1.3f, seed);

        {
            auto shadow = p;
            shadow.applyTransform (juce::AffineTransform::translation (3.0f, 3.5f));
            g.setColour (colours::cardBorder.withAlpha (0.22f));
            g.fillPath (shadow);
        }

        g.setColour (fill);
        g.fillPath (p);

        g.setColour (border);
        g.strokePath (p, juce::PathStrokeType (strokeWidth(), juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));
    }

    //==========================================================================
    void drawWeightedText (juce::Graphics& g, const juce::String& text, const juce::Font& font,
                           juce::Rectangle<float> area, juce::Justification just,
                           juce::Colour fill, float extraWeight, float shear,
                           juce::Colour outline)
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

        if (outline != juce::Colour())
        {
            g.setColour (outline);
            g.strokePath (p, juce::PathStrokeType (extraWeight + 5.0f,
                                                   juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));
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

        p.startNewSubPath (x + w * 0.14f, y + h * 0.44f);
        p.lineTo          (x + w * 0.10f, y + h * 0.02f);
        p.lineTo          (x + w * 0.42f, y + h * 0.20f);
        p.closeSubPath();

        p.startNewSubPath (x + w * 0.86f, y + h * 0.44f);
        p.lineTo          (x + w * 0.90f, y + h * 0.02f);
        p.lineTo          (x + w * 0.58f, y + h * 0.20f);
        p.closeSubPath();

        p.addEllipse (x + w * 0.06f, y + h * 0.18f, w * 0.88f, h * 0.78f);

        g.setColour (colour);
        g.fillPath (p);

        g.setColour (juce::Colours::white.withAlpha (colour.getFloatAlpha() * 0.92f));
        g.fillEllipse (x + w * 0.28f, y + h * 0.48f, w * 0.11f, h * 0.15f);
        g.fillEllipse (x + w * 0.61f, y + h * 0.48f, w * 0.11f, h * 0.15f);
    }

    void drawCurledCat (juce::Graphics& g, juce::Rectangle<float> area)
    {
        auto c = area.getCentre();

        juce::ColourGradient grad (colours::knobFaceHi,
                                   c.x - area.getWidth() * 0.22f,
                                   c.y - area.getHeight() * 0.28f,
                                   colours::knobFaceLo,
                                   c.x + area.getWidth() * 0.32f,
                                   c.y + area.getHeight() * 0.40f,
                                   true);
        grad.addColour (0.45, juce::Colour (0xff231a15));

        g.setGradientFill (grad);
        g.fillEllipse (area);

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

        g.setColour (juce::Colours::white.withAlpha (0.06f));
        g.fillEllipse (area.getX() + area.getWidth() * 0.18f,
                       area.getY() + area.getHeight() * 0.10f,
                       area.getWidth() * 0.34f, area.getHeight() * 0.22f);
    }

    //==========================================================================
    juce::Path chevron (juce::Rectangle<float> area, float rotationRadians)
    {
        juce::Path p;
        p.startNewSubPath (-0.5f,  -0.28f);
        p.lineTo          ( 0.0f,   0.22f);
        p.lineTo          ( 0.5f,  -0.28f);

        juce::Path stroked;
        juce::PathStrokeType (0.16f, juce::PathStrokeType::curved,
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

        p.startNewSubPath (x,             y + h * 0.10f);
        p.lineTo          (x + w,         y + h * 0.10f);
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
        juce::Path p, ring;
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

    juce::Path boltIcon (juce::Rectangle<float> area)
    {
        juce::Path p;
        const auto x = area.getX(), y = area.getY(), w = area.getWidth(), h = area.getHeight();

        p.startNewSubPath (x + w * 0.58f, y);
        p.lineTo          (x + w * 0.10f, y + h * 0.56f);
        p.lineTo          (x + w * 0.44f, y + h * 0.56f);
        p.lineTo          (x + w * 0.34f, y + h);
        p.lineTo          (x + w * 0.92f, y + h * 0.40f);
        p.lineTo          (x + w * 0.54f, y + h * 0.40f);
        p.closeSubPath();
        return p;
    }

    juce::Path sparkle (juce::Point<float> centre, float radius)
    {
        juce::Path p;
        const float k = radius * 0.28f;

        p.startNewSubPath (centre.x,     centre.y - radius);
        p.quadraticTo     (centre.x + k, centre.y - k, centre.x + radius, centre.y);
        p.quadraticTo     (centre.x + k, centre.y + k, centre.x,          centre.y + radius);
        p.quadraticTo     (centre.x - k, centre.y + k, centre.x - radius, centre.y);
        p.quadraticTo     (centre.x - k, centre.y - k, centre.x,          centre.y - radius);
        p.closeSubPath();
        return p;
    }
}
