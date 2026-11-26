cmake_minimum_required(VERSION 3.22)

project(Rubbish VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version")

include(cmake/CPM.cmake)

# Use the local JUCE checkout instead of downloading it.
# Override on another machine with: cmake -DCPM_JUCE_SOURCE=/path/to/JUCE
set(CPM_JUCE_SOURCE "$ENV{HOME}/JUCE" CACHE PATH "Local JUCE 9.0.5 checkout")

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 9.0.5
)

juce_add_plugin(Rubbish
    COMPANY_NAME "Rubbish"
    BUNDLE_ID "com.rubbish.Rubbish"
    PLUGIN_MANUFACTURER_CODE Rbsh
    PLUGIN_CODE Rbsh
    PRODUCT_NAME "Rubbish"
    FORMATS AU VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    AU_MAIN_TYPE kAudioUnitType_Effect
    VST3_CATEGORIES Fx Distortion
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(Rubbish PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Parameters.cpp
    Source/TwistParameterReader.cpp
    Source/LfoParameterReader.cpp
    Source/StateMigration.cpp
    Source/dsp/Waveshapers.cpp
    Source/dsp/CharacterShaper.cpp
    Source/dsp/DistortionStage.cpp
    Source/dsp/ResonantFilter.cpp
    Source/dsp/FilterSection.cpp
    Source/dsp/FilterOrderSwitch.cpp
    Source/dsp/LoudnessMatcher.cpp
    Source/dsp/LoudnessTracker.cpp
    Source/dsp/CleanLowSplitter.cpp
    Source/dsp/StereoSpread.cpp
    Source/dsp/FilterFollow.cpp
    Source/dsp/DynamicsRestore.cpp
    Source/dsp/SafetyLimiter.cpp
    Source/dsp/OutputSection.cpp
    Source/dsp/Lfo.cpp
    Source/dsp/LfoBank.cpp
    Source/dsp/ModulationMatrix.cpp
    Source/dsp/TwistClock.cpp
    Source/dsp/TwistBell.cpp
    Source/dsp/TwistStage.cpp
    Source/ui/LabelledKnob.cpp
    Source/ui/FineDragSlider.cpp
    Source/ui/PanelStyle.cpp
    Source/ui/DrivePanel.cpp
    Source/ui/FiltersPanel.cpp
    Source/ui/FilterOrderSwapButton.cpp
    Source/ui/MainLayout.cpp
    Source/ui/LevelMeter.cpp
    Source/ui/OutputPanel.cpp
    Source/ui/TwistBellControls.cpp
    Source/ui/TwistPanel.cpp
    Source/ui/TwistDisplay.cpp
)

target_include_directories(Rubbish PRIVATE Source)

target_compile_definitions(Rubbish PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Rubbish
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
