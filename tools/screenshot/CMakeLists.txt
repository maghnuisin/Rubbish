# Renders Rubbish's editor to a PNG without a DAW (used for the README screenshot).
# Build: cmake -S tools/screenshot -B tools/screenshot/build -DCMAKE_BUILD_TYPE=Release && cmake --build tools/screenshot/build
cmake_minimum_required(VERSION 3.22)
project(RubbishScreenshot VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RUBBISH_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(${RUBBISH_ROOT}/cmake/CPM.cmake)
set(CPM_JUCE_SOURCE "$ENV{HOME}/JUCE" CACHE PATH "Local JUCE 9.0.5 checkout")
CPMAddPackage(NAME JUCE GITHUB_REPOSITORY juce-framework/JUCE GIT_TAG 9.0.5)

juce_add_console_app(RubbishScreenshot PRODUCT_NAME "RubbishScreenshot")

file(GLOB RUBBISH_SOURCES CONFIGURE_DEPENDS
    ${RUBBISH_ROOT}/Source/*.cpp ${RUBBISH_ROOT}/Source/dsp/*.cpp ${RUBBISH_ROOT}/Source/ui/*.cpp)
target_sources(RubbishScreenshot PRIVATE main.cpp ${RUBBISH_SOURCES})
target_include_directories(RubbishScreenshot PRIVATE ${RUBBISH_ROOT}/Source)
target_compile_definitions(RubbishScreenshot PRIVATE
    JucePlugin_Name="Rubbish" JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_MODAL_LOOPS_PERMITTED=1)
target_link_libraries(RubbishScreenshot PRIVATE
    juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)
