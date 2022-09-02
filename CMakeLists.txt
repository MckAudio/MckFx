cmake_minimum_required(VERSION 3.15)

project(MCK_DELAY VERSION 0.0.1)

add_subdirectory(JUCE)

juce_add_plugin(MckDelayPlugin
    VERSION 0.0.1
    COMPANY_NAME MckAudio
    PLUGIN_MANUFACTURER_CODE MckA
    PLUGIN_CODE McDl
    FORMATS AU VST3 LV2 Standalone
    PRODUCT_NAME "MckDelay"
    LV2URI https://github.com/MckAudio/MckDelay
    COPY_PLUGIN_AFTER_BUILD TRUE)

juce_generate_juce_header(MckDelayPlugin)

target_sources(MckDelayPlugin
    PRIVATE
    ./Source/BwLookAndFeel.cpp
    ./Source/BwLookAndFeel.hpp
    ./Source/Control.hpp
    ./Source/DelayModule.cpp
    ./Source/DelayModule.hpp
    ./Source/MckLookAndFeel.cpp
    ./Source/MckLookAndFeel.hpp
    ./Source/PluginEditor.cpp
    ./Source/PluginEditor.hpp
    ./Source/PluginProcessor.cpp
    ./Source/PluginProcessor.hpp
)

target_compile_definitions(MckDelayPlugin
    PRIVATE
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MckDelayPlugin
    PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)