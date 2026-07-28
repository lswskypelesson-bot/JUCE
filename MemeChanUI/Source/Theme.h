#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/*  Central palette, typography and shared drawing helpers.

    Two skins are supported. `modern` is the flat, soft-shadow look; `comic` is
    the riso/screentone print look - cream stock, heavy ink outlines, halftone
    and a slight misregistration offset. Widgets do not branch on the skin
    themselves: they read colours::* and call drawCard()/strokeWidth(), both of
    which follow whatever skin is active.
*/
namespace theme
{
    enum class Skin { modern, comic };

    void setSkin (Skin);
    Skin currentSkin() noexcept;

    /** Outline weight for the active skin - 1px flat, 2.2px inked. */
    float strokeWidth() noexcept;

    namespace colours
    {
        extern juce::Colour windowBg, cardBg, cardBorder, panelBg;
        extern juce::Colour textPrimary, textSecondary, textTertiary;
        extern juce::Colour accent, accentHover, accentSoft;
        extern juce::Colour heroBlue, heroBlueDeep, heroStreak, heroPaper;
        extern juce::Colour knobRimHi, knobRimLo, knobFaceHi, knobFaceLo;
        extern juce::Colour meterTrack, meterLo, meterHi;
        extern juce::Colour catBlack;
    }

    /** Latin UI face. The mock-ups use an Inter-like grotesque; this box only
        has Liberation Sans, so that is what the render uses. */
    juce::Font sans (float height, bool bold = false, bool italic = false);

    /** Japanese face - IPAGothic is the only CJK family available here. */
    juce::Font jp (float height);

    /** Small all-caps section labels ("AMP MODEL", "EQ", ...). */
    juce::Font label (float height);

    /** Panel background: soft shadow under the modern skin, inked outline and
        offset plate under the comic skin. */
    void drawCard (juce::Graphics&, juce::Rectangle<float> bounds, float corner = 12.0f,
                   juce::Colour fill = {}, juce::Colour border = {});

    /** Rounded rectangle with the corners and edges nudged off true, so an
        outline reads as drawn rather than generated. */
    juce::Path roughRoundedRect (juce::Rectangle<float>, float corner,
                                 float jitter, int seed);

    /** How much of an outline the press failed to lay down, 0 (solid) to about
        0.5 (heavily broken). Ignored by the modern skin. */
    float inkBreakup() noexcept;
    void setInkBreakup (float) noexcept;

    /** Strokes a path as dry, broken ink: the line is eroded along its length
        and leaves the odd speck behind in the gaps.

        The result is pure geometry, so it stays sharp at any zoom - but it is
        rebuilt on every call, so components using it should be buffered
        (Component::setBufferedToImage) rather than repainted per frame.

        Pass `amount` to override the global breakup for one shape - small
        controls need less erosion than a full-width panel to read cleanly. */
    void strokeInked (juce::Graphics&, const juce::Path& source, juce::Colour,
                      float width, int seed, float amount = -1.0f);

    /** Screentone. `spacing` is dot pitch, `radius` dot size. */
    void halftone (juce::Graphics&, juce::Rectangle<float> area, juce::Colour,
                   float spacing, float radius, float angleRadians = 0.0f);

    /** Tiled paper grain, cached after the first call. Draws nothing under the
        modern skin. */
    void paperGrain (juce::Graphics&, juce::Rectangle<float> area);

    /** Fills text as a path so it can be given extra weight / slant, which the
        available fonts cannot provide on their own. */
    void drawWeightedText (juce::Graphics&, const juce::String& text, const juce::Font&,
                           juce::Rectangle<float> area, juce::Justification,
                           juce::Colour fill, float extraWeight = 0.0f,
                           float shear = 0.0f, juce::Colour outline = {});

    /** Placeholder cat mark, stands in for the logo artwork. */
    void drawCat (juce::Graphics&, juce::Rectangle<float> area, juce::Colour);

    /** Curled-up cat blob used on the knob faces and in the hero banner. */
    void drawCurledCat (juce::Graphics&, juce::Rectangle<float> area);

    juce::Path chevron (juce::Rectangle<float> area, float rotationRadians);
    juce::Path gearIcon (juce::Rectangle<float> area);
    juce::Path funnelIcon (juce::Rectangle<float> area);
    juce::Path waveIcon (juce::Rectangle<float> area);
    juce::Path headphoneIcon (juce::Rectangle<float> area);
    juce::Path smileyIcon (juce::Rectangle<float> area);
    juce::Path boltIcon (juce::Rectangle<float> area);
    juce::Path sparkle (juce::Point<float> centre, float radius);
}
