#include "MemeChanEditor.h"

/*  Offline renderer.

    Builds the editor component, paints each tab into an image and writes one
    PNG per page. No window is opened, so this runs under a bare X server
    (Xvfb) and gives a quick way to review layout changes without loading the
    plugin in a host.

        MemeChanUIRender <out-dir> [scale]
*/
namespace
{
    bool renderTo (MemeChanEditor& editor, const juce::File& out, float scale)
    {
        juce::Image image (juce::Image::ARGB,
                           juce::roundToInt ((float) editor.getWidth()  * scale),
                           juce::roundToInt ((float) editor.getHeight() * scale),
                           true);

        {
            juce::Graphics g (image);
            g.addTransform (juce::AffineTransform::scale (scale));
            editor.paintEntireComponent (g, true);
        }

        out.deleteFile();
        juce::FileOutputStream stream (out);

        if (! stream.openedOk())
        {
            std::cerr << "could not open " << out.getFullPathName() << std::endl;
            return false;
        }

        juce::PNGImageFormat png;

        if (! png.writeImageToStream (image, stream))
        {
            std::cerr << "PNG encode failed for " << out.getFullPathName() << std::endl;
            return false;
        }

        std::cout << out.getFullPathName() << " (" << image.getWidth() << "x"
                  << image.getHeight() << ")" << std::endl;
        return true;
    }
}

int main (int argc, char** argv)
{
    const juce::ScopedJuceInitialiser_GUI juceInit;

    const juce::String dirArg = argc > 1 ? juce::String (argv[1]) : juce::String (".");
    const float scale = argc > 2 ? juce::String (argv[2]).getFloatValue() : 1.0f;

    const auto dir = juce::File::isAbsolutePath (dirArg)
                       ? juce::File (dirArg)
                       : juce::File::getCurrentWorkingDirectory().getChildFile (dirArg);
    dir.createDirectory();

    const char* pageNames[] = { "amp", "drive", "fx", "routing" };

    const struct { theme::Skin skin; const char* prefix; } skins[] =
    {
        { theme::Skin::modern, "preview-" },
        { theme::Skin::comic,  "preview-comic-" },
    };

    for (const auto& s : skins)
    {
        // the palette is global, so the editor is rebuilt per skin
        theme::setSkin (s.skin);

        MemeChanEditor editor;
        editor.setSize (MemeChanEditor::designWidth, MemeChanEditor::designHeight);

        for (int i = 0; i < juce::numElementsInArray (pageNames); ++i)
        {
            editor.showPage (i);

            if (! renderTo (editor, dir.getChildFile (juce::String (s.prefix)
                                                          + pageNames[i] + ".png"), scale))
                return 1;
        }
    }

    return 0;
}
