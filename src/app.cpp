
#include "app.hpp"

#include <iostream>

namespace padauk {

void App::triggerNote(const unsigned char note) {
    synthManager.voice()->setValue(Marimba::Parameter::MidiNote, note);
    synthManager.triggerOn(note);
}

void App::onCreate() {
    // Disable keyboard navigation.
    navControl().active(false);
    // Set Gamma sampling rate from Allolib app's audio.
    gam::sampleRate(audioIO().framesPerSecond());
}

void App::onInit() {
    al::imguiInit();

    auto *const voice = synthManager.voice();

    voice->setValue(Marimba::Parameter::VisualWidth, width());

    if (midiIn.getPortCount() > 0) {
        // Bind MIDI handler if there is a MIDI device connected.
        MIDIMessageHandler::bindTo(midiIn);

        // Open the last MIDI device.
        const unsigned int port = midiIn.getPortCount() - 1;
        midiIn.openPort(port);
        std::cerr << "Opened MIDI port to " << midiIn.getPortName(port) << "."
                  << std::endl;
    } else {
        std::cerr << "Could not find a MIDI device to connect to." << std::endl;
    }
}

void App::onResize(const int w, const int h) {
    auto *const voice = synthManager.voice();
    voice->setValue(Marimba::Parameter::VisualWidth, w);
    voice->setValue(Marimba::Parameter::VisualHeight, h);
}

void App::onSound(al::AudioIOData &io) {
    // if (useCompressor) {
    //     compressor(io);
    // }

    synthManager.render(io);
}

void App::onDraw(al::Graphics &g) {
    g.clear();
    g.camera(al::Viewpoint::ORTHO_FOR_2D);
    synthManager.render(g);
    al::imguiDraw();
}

void App::onAnimate(const double _dt) {
    al::imguiBeginFrame();

    // Draw synth control panel.
    synthManager.drawFields();
    synthManager.drawSynthSequencer();
    synthManager.drawSynthRecorder();

    al::imguiEndFrame();
}

bool App::onKeyDown(const al::Keyboard &k) {
    if (al::ParameterGUI::usingKeyboard()) {
        // Ignore keypresses while the keyboard is controlling the control
        // panel.
        return true;
    }

    const int key = k.key();
    switch (key) {
    case al::Keyboard::RIGHT:
        keyboardParameters.octaveOffset++;
        break;
    case al::Keyboard::LEFT:
        keyboardParameters.octaveOffset--;
        break;
    // case '`':
    //     useCompressor = !useCompressor;
    //     std::cerr << std::boolalpha << "Use compressor? " << useCompressor
    //               << std::endl;
    //     break;
    default:
        if (('a' <= key && 'z' >= key) || ('0' <= key && '9' >= key)) {
            triggerNote(al::asciiToMIDI(key) +
                        12 * keyboardParameters.octaveOffset);
        }
    }

    return true;
}

bool App::onKeyUp(const al::Keyboard &k) {
    const int key = k.key();
    if (('a' <= key && 'z' >= key) || ('0' <= key && '9' >= key)) {
        const int midiNote = al::asciiToMIDI(key);
        if (midiNote > 0) {
            synthManager.triggerOff(midiNote);
        }
    }
    return true;
}

void App::onMIDIMessage(const al::MIDIMessage &m) {
    auto *const voice = synthManager.voice();
    const unsigned char midiNote = m.noteNumber();

    switch (m.type()) {
    case al::MIDIByte::NOTE_ON: {
        const double velocity = m.velocity();
        if (midiNote > 0 && velocity > 0.001) {
            voice->setValue(Marimba::Parameter::Amplitude, velocity);
            triggerNote(midiNote);
        }
        break;
    }
    case al::MIDIByte::NOTE_OFF:
        synthManager.triggerOff(midiNote);
        break;
    case al::MIDIByte::CONTROL_CHANGE: {
        const double controlValue = m.controlValue();
        switch (m.controlNumber()) {
        case 7:
            voice->setValue(Marimba::Parameter::Hardness, controlValue * 12.f);
            break;
        case 11:
            voice->setValue(Marimba::Parameter::Brightness,
                            controlValue * 12.f);
            break;
        case 14:
            break;
        case 15:
            break;
        }
        break;
    }
    }
}

void App::onExit() { al::imguiShutdown(); }

}; // namespace padauk