#include "MemeChanEditor.h"

/*  Offline renderer.

    Builds the editor component, paints it into an image and writes a PNG.
    No window is opened, so this runs under a bare X server (Xvfb) and gives a
    quick way to review layout changes without loading the plugin in a host.

        MemeChanUIRender <out.png> [scale]
*/
int main (int argc, char** argv)
{
    const juce::ScopedJuceInitialiser_GUI juceInit;

    const juce::String outPath = argc > 1 ? juce::String (argv[1]) : juce::String ("memechan-ui.png");
    const float scale = argc > 2 ? juce::String (argv[2]).getFloatValue() : 1.0f;

    MemeChanEditor editor;
    editor.setSize (MemeChanEditor::designWidth, MemeChanEditor::designHeight);

    juce::Image image (juce::Image::ARGB,
                       juce::roundToInt ((float) editor.getWidth()  * scale),
                       juce::roundToInt ((float) editor.getHeight() * scale),
                       true);

    {
        juce::Graphics g (image);
        g.addTransform (juce::AffineTransform::scale (scale));
        editor.paintEntireComponent (g, true);
    }

    const auto out = juce::File::isAbsolutePath (outPath)
                       ? juce::File (outPath)
                       : juce::File::getCurrentWorkingDirectory().getChildFile (outPath);

    out.deleteFile();

    juce::FileOutputStream stream (out);

    if (! stream.openedOk())
    {
        std::cerr << "could not open " << out.getFullPathName() << std::endl;
        return 1;
    }

    juce::PNGImageFormat png;

    if (! png.writeImageToStream (image, stream))
    {
        std::cerr << "PNG encode failed" << std::endl;
        return 1;
    }

    std::cout << out.getFullPathName() << " (" << image.getWidth() << "x"
              << image.getHeight() << ")" << std::endl;
    return 0;
}
