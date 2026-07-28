#pragma once

#include "Theme.h"

/*  The illustrated header.

    Everything here is drawn procedurally so the layout can be reviewed before
    the real artwork exists. In the shipping build this component becomes a
    handful of BinaryData images (background plate, cat cut-out, logo lockup)
    composited at the same positions.
*/
class HeroBanner final : public juce::Component
{
public:
    HeroBanner();
    void paint (juce::Graphics&) override;

private:
    void paintSpeedLines (juce::Graphics&, juce::Rectangle<float>) const;
    void paintBurst (juce::Graphics&, juce::Rectangle<float>) const;
    void paintLogoText (juce::Graphics&, juce::Rectangle<float>) const;
    void paintCat (juce::Graphics&, juce::Rectangle<float>) const;

    juce::Path burstPath;
};
