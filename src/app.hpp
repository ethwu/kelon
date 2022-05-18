
#ifndef PADAUK_APP_H
#define PADAUK_APP_H

#include "al/app/al_App.hpp"
#include "al/ui/al_ControlGUI.hpp"

#include "marimba.hpp"

namespace padauk {

/// Parameters controlling how computer keypresses are handled.
struct KeyboardParameters {
    int octaveOffset = 0;
};

/// Marimba demo application.
class App : public al::App, al::MIDIMessageHandler {
private:
    /// Manages synth voices and their associated graphics.
    al::SynthGUIManager<Marimba> synthManager{"padauk"};

    /// MIDI input.
    RtMidiIn midiIn;

    // /// Compressor.
    // padauk::CompressorPlugin<padauk::BLOCK_SIZE> compressor;
    // /// Whether to compress audio.
    // bool useCompressor = false;

    /// Keyboard parameters.
    KeyboardParameters keyboardParameters{};

    /// Trigger a given MIDI note.
    void triggerNote(const unsigned char note);

    void onCreate() override;
    void onInit() override;
    void onResize(const int w, const int h) override;
    void onSound(al::AudioIOData &io) override;
    void onDraw(al::Graphics &g) override;
    void onAnimate(const double dt) override;
    bool onKeyDown(const al::Keyboard &k) override;
    bool onKeyUp(const al::Keyboard &k) override;
    void onMIDIMessage(const al::MIDIMessage &m) override;
    void onExit() override;
};

}; // namespace padauk

#endif
