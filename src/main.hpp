
#ifndef MAIN_H
#define MAIN_H

#include "al/app/al_App.hpp"
#include "al/graphics/al_Font.hpp"
#include "al/ui/al_ControlGUI.hpp"

#include "compressor.hpp"
#include "marimba.hpp"

/// The helper function used to visualize which keys pressed or released on a
/// virtual piano.
int asciiToKeyLabelIndex(int asciiKey);

class MyApp : public al::App, al::MIDIMessageHandler {
public:
    // GUI manager for SineEnv voices
    // The name provided determines the name of the directory
    // where the presets and sequences are stored
    al::SynthGUIManager<Marimba> synthManager{"Marimba"};

    RtMidiIn midiIn;

    // Compressor.
    CompressorPlugin<BLOCK_SIZE> compressor;
    bool useCompressor = false;

    // Octave offset.
    int offset = 0;
    // Whether to hold notes or not.
    bool holdNotes = true;

    // This function is called right after the window is created
    // It provides a grphics context to initialize ParameterGUI
    // It's also a good place to put things that should
    // happen once at startup.
    void onCreate() override;

    void onInit() override;

    // The audio callback function. Called when audio hardware requires data
    void onSound(al::AudioIOData &io) override;

    void onAnimate(double dt) override;

    // The graphics callback function.
    void onDraw(al::Graphics &g) override;

    // Trigger a note.
    void triggerNote(const unsigned int note);

    // Whenever a key is pressed, this function is called
    bool onKeyDown(al::Keyboard const &k) override;

    // Whenever a key is released this function is called
    bool onKeyUp(al::Keyboard const &k) override;

    // Callback for MIDI messages.
    void onMIDIMessage(const al::MIDIMessage &m) override;

    // Whenever the window size changes this function is called
    void onResize(int w, int h) override;

    void onExit() override;
};

#endif
