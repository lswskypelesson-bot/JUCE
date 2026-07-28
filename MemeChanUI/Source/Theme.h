#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/*  Central palette, typography and a handful of shared drawing helpers.

    Everything the UI draws goes through here, so re-skinning the plugin means
    editing this file rather than hunting through the widgets.
*/
namespace theme
{
    namespace colours
    {
        const juce::Colour windowBg     { 0xfff5f5f7 };
        const juce::Colour cardBg       { 0xffffffff };
        const juce::Colour cardBorder   { 0xffe4e6ea };
        const juce::Colour panelBg      { 0xfffbfbfc };

        const juce::Colour textPrimary  { 0xff191c22 };
        const juce::Colour textSecondary{ 0xff8f959f };
        const juce::Colour textTertiary { 0xffb6bbc4 };

        const juce::Colour accent       { 0xff3b5fe0 };
        const juce::Colour accentHover  { 0xff2f4fc7 };
        const juce::Colour accentSoft   { 0xffe9effc };

        const juce::Colour heroBlue     { 0xff5385d8 };
        const juce::Colour heroBlueDeep { 0xff3f6cc4 };
        const juce::Colour heroStreak   { 0xffc9dcf5 };
        const juce::Colour heroPaper    { 0xfffcfaf4 };

        const juce::Colour knobRimHi    { 0xfffefdfa };
        const juce::Colour knobRimLo    { 0xffd4cec1 };
        const juce::Colour knobFaceHi   { 0xff4a3b31 };
        const juce::Colour knobFaceLo   { 0xff0d0a09 };

        const juce::Colour meterTrack   { 0xffe8eaee };
        const juce::Colour meterLo      { 0xff6f8ef0 };
        const juce::Colour meterHi      { 0xff3b5fe0 };

        const juce::Colour catBlack     { 0xff17181c };
    }

    /** Latin UI face. The mock uses an Inter-like grotesque; this box only has
        Liberation Sans, so that is what the render uses. */
    juce::Font sans (float height, bool bold = false, bool italic = false);

    /** Japanese face - IPAGothic is the only CJK family available here. */
    juce::Font jp (float height);

    /** Small all-caps section labels ("AMP MODEL", "EQ", ...). */
    juce::Font label (float height);

    /** White card with a hairline border and a soft drop shadow. */
    void drawCard (juce::Graphics&, juce::Rectangle<float> bounds, float corner = 12.0f,
                   juce::Colour fill = colours::cardBg,
                   juce::Colour border = colours::cardBorder);

    /** Fills text as a path so it can be given extra weight / slant, which the
        available fonts cannot provide on their own. */
    void drawWeightedText (juce::Graphics&, const juce::String& text, const juce::Font&,
                           juce::Rectangle<float> area, juce::Justification,
                           juce::Colour fill, float extraWeight = 0.0f,
                           float shear = 0.0f);

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
    juce::Path sparkle (juce::Point<float> centre, float radius);
}
