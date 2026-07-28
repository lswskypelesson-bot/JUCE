cmake_minimum_required(VERSION 3.22)

project(MemeChanUI VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This project lives inside the JUCE tree, so pull JUCE in from the parent
# directory. An explicit binary dir is required because the source dir is
# not below this one.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/JUCE EXCLUDE_FROM_ALL)

# Offline renderer: builds the editor component and writes a PNG. No window,
# no audio device - just the GUI code, so the layout can be reviewed quickly.
juce_add_console_app(MemeChanUIRender
    PRODUCT_NAME "MemeChanUIRender")

target_sources(MemeChanUIRender PRIVATE
    Source/Theme.cpp
    Source/Widgets.cpp
    Source/HeroBanner.cpp
    Source/MemeChanEditor.cpp
    Source/RenderMain.cpp)

target_compile_definitions(MemeChanUIRender PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_STANDALONE_APPLICATION=1
    JUCE_DISABLE_JUCE_VERSION_PRINTING=1)

target_link_libraries(MemeChanUIRender PRIVATE
    juce::juce_gui_basics
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)
