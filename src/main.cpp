
#include "main.hpp"

#include "al/graphics/al_Shapes.hpp"

#include <iostream>

#include "util.hpp"

void MyApp::onCreate() {
    navControl().active(false); // Disable navigation via keyboard, since we
                                // will be using keyboard for note triggering

    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());
}

void MyApp::onInit() {
    al::imguiInit();

    auto *voice = synthManager.voice();

    voice->setValue(Marimba::Parameter::VisualWidth, width());
    voice->setValue(Marimba::Parameter::VisualHeight, height());

    if (midiIn.getPortCount() > 0) {
        // Bind MIDI handler if there is a MIDI device connected.
        MIDIMessageHandler::bindTo(midiIn);

        // Open the last MIDI device.
        const unsigned int port = midiIn.getPortCount() - 1;
        midiIn.openPort(port);
        std::cerr << "Opened port to " << midiIn.getPortName(port) << std::endl;
    } else {
        std::cerr << "Did not find a MIDI device to connect to." << std::endl;
    }

    // Play example sequence. Comment this line to start from scratch
    synthManager.synthSequencer().playSequence("synth1.synthSequence");
    synthManager.synthRecorder().verbose(true);
}

void MyApp::onSound(al::AudioIOData &io) {
    if (useCompressor) compressor(io);

    synthManager.render(io); // Render audio
}

void MyApp::onAnimate(double dt) {
    // The GUI is prepared here
    al::imguiBeginFrame();
    // Draw a window that contains the synth control panel
    synthManager.drawFields();
    synthManager.drawSynthSequencer();
    synthManager.drawSynthRecorder();
    al::imguiEndFrame();
}

void MyApp::onDraw(al::Graphics &g) {
    g.clear();
    g.camera(al::Viewpoint::ORTHO_FOR_2D); // Ortho [0:width] * [0:height]

    // Render the synth's graphics
    synthManager.render(g);
    // GUI is drawn here
    al::imguiDraw();
}

void MyApp::triggerNote(const unsigned int note) {
    auto *voice = synthManager.voice();
    voice->setValue(Marimba::Parameter::MidiNote, note);
    synthManager.triggerOn(note);
}

bool MyApp::onKeyDown(al::Keyboard const &k) {
    if (al::ParameterGUI::usingKeyboard()) { // Ignore keys if GUI is using
                                             // keyboard
        return true;
    }

    auto *voice = synthManager.voice();
    switch (k.key()) {
    case '=':
        useCompressor = !useCompressor;
        std::cerr << "use compressor: " << useCompressor << std::endl;
        break;
    case al::Keyboard::Key::BACKSPACE:
        holdNotes = !holdNotes;
        std::cerr << "hold notes: " << holdNotes << std::endl;
        break;
    case '[':
        voice->xylophone();
        break;
    case ']':
        voice->marimba();
        break;
    case al::Keyboard::Key::RIGHT:
        // Increase octave by one on right arrow.
        offset++;
        break;
    case al::Keyboard::Key::LEFT:
        // Reduce octave by one on left arrow.
        offset--;
        break;
    case al::Keyboard::Key::UP:
        voice->setValue(Marimba::Parameter::Hardness,
                        voice->value(Marimba::Parameter::Hardness) + 1);
        break;
    case al::Keyboard::Key::DOWN:
        voice->setValue(Marimba::Parameter::Hardness,
                        voice->value(Marimba::Parameter::Hardness) - 1);
        break;
    default:
        // Use the built-in allolib ASCII to MIDI converter, along with the
        // offset above.
        triggerNote(al::asciiToMIDI(k.key()) + 12 * offset);
        break;
    }
    return true;
}

bool MyApp::onKeyUp(al::Keyboard const &k) {
    int midiNote = al::asciiToMIDI(k.key());
    if (midiNote > 0) {
        synthManager.triggerOff(midiNote);
    }
    return true;
}

void MyApp::onMIDIMessage(const al::MIDIMessage &m) {
    auto *voice = synthManager.voice();
    switch (m.type()) {
    case al::MIDIByte::NOTE_ON: {
        int midiNote = m.noteNumber();
        if (midiNote > 0 && m.velocity() > 0.001) {
            voice->setValue(Marimba::Parameter::Amplitude, m.velocity());
            triggerNote(midiNote);
        }
        break;
                                }
    case al::MIDIByte::NOTE_OFF: {
        int midiNote = m.noteNumber();
        // printf("Note OFF %u, Vel %f\n", m.noteNumber(), m.velocity());
        if (!holdNotes || true)
            synthManager.triggerOff(midiNote);
        break;
                                 }
    case al::MIDIByte::CONTROL_CHANGE:
        // std::cerr << "cc " << int(m.controlNumber()) << ": " <<
        // m.controlValue()
        //   << std::endl;
        switch (m.controlNumber()) {
        case 7:
            voice->setValue(Marimba::Parameter::Hardness,
                            m.controlValue() * 12.f);
            break;
        case 11:
            voice->setValue(Marimba::Parameter::Brightness,
                            m.controlValue() * 12.f);
            break;
        case 14:
            break;
        case 15:
            break;
        }
        break;

    default:;
    }
}

void MyApp::onResize(int w, int h) {
    auto *voice = synthManager.voice();
    voice->setValue(Marimba::Parameter::VisualWidth, w);
    voice->setValue(Marimba::Parameter::VisualHeight, h);
}

void MyApp::onExit() { al::imguiShutdown(); }

int main() {
    // Create app instance
    MyApp app;

    // Set window size
    app.dimensions(1200, 600);

    // Set up audio
    app.configureAudio(48000., 512, 2, 0);
    app.start();

    return 0;
}
