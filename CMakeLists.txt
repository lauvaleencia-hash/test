cmake_minimum_required(VERSION 3.22)
project(FRACTURE_BLOOM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.5
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(FRACTURE_BLOOM
  target_link_libraries(FRACTURE_BLOOM
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_processors
)
  COMPANY_NAME "Bloom Audio Labs"
  BUNDLE_ID com.bloomaudiolabs.fracturebloom
  PLUGIN_MANUFACTURER_CODE BmAL
  PLUGIN_CODE FrBl
  FORMATS AU VST3
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  PRODUCT_NAME "FRACTURE BLOOM"
)

target_sources(FRACTURE_BLOOM PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginProcessor.h
  Source/PluginEditor.cpp
  Source/PluginEditor.h
  Source/DSP/GranularEngine.h
  Source/DSP/SpectralEngine.h
  Source/DSP/DistortionEngine.h
  Source/DSP/ModulationEngine.h
)

target_compile_definitions(FRACTURE_BLOOM
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(FRACTURE_BLOOM
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
