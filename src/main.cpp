
#include "main.hpp"

#include "al/graphics/al_Shapes.hpp"

#include <iostream>

#include "util.hpp"

int asciiToKeyLabelIndex(int asciiKey) {
    switch (asciiKey) {
    case '2':
        return 30;
    case '3':
        return 31;
    case '5':
        return 33;
    case '6':
        return 34;
    case '7':
        return 35;
    case '9':
        return 37;
    case '0':
        return 38;

    case 'q':
        return 10;
    case 'w':
        return 11;
    case 'e':
        return 12;
    case 'r':
        return 13;
    case 't':
        return 14;
    case 'y':
        return 15;
    case 'u':
        return 16;
    case 'i':
        return 17;
    case 'o':
        return 18;
    case 'p':
        return 19;

    case 's':
        return 20;
    case 'd':
        return 21;
    case 'g':
        return 23;
    case 'h':
        return 24;
    case 'j':
        return 25;
    case 'l':
        return 27;
    case ';':
        return 28;

    case 'z':
        return 0;
    case 'x':
        return 1;
    case 'c':
        return 2;
    case 'v':
        return 3;
    case 'b':
        return 4;
    case 'n':
        return 5;
    case 'm':
        return 6;
    case ',':
        return 7;
    case '.':
        return 8;
    case '/':
        return 9;
    }
    return 0;
}

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
    synthManager.render(io); // Render audio
    if (useCompressor) {
        compressor(io);
    }
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

    // This example uses only the orthogonal projection for 2D drawing
    g.camera(al::Viewpoint::ORTHO_FOR_2D); // Ortho [0:width] * [0:height]

    // // Drawing white piano keys
    // for (int i = 0; i < 20; i++) {
    //     int index = i % 10;
    //     g.pushMatrix();

    //     float c = 0.9;
    //     float x = (keyWidth + keyPadding * 2) * index + keyPadding;
    //     float y = 0;

    //     if (i >= 10) {
    //         y = keyHeight + keyPadding * 2;
    //     }

    //     g.translate(x, y);
    //     g.color(c);
    //     g.tint(c);
    //     g.draw(meshKey);

    //     g.tint(1);
    //     fontRender.write(whitekeyLabels[i].c_str(), fontSize);
    //     fontRender.renderAt(g, {keyWidth * 0.5 - 5, keyHeight * 0.1, 0.0});

    //     g.popMatrix();
    // }

    // // Drawing balck piano keys
    // for (int i = 0; i < 20; i++) {
    //     int index = i % 10;
    //     if (index == 2 || index == 6 || index == 9)
    //         continue;

    //     g.pushMatrix();

    //     float c = 0.5;
    //     float x =
    //         (keyWidth + keyPadding * 2) * index + keyPadding + keyWidth *
    //         0.5;
    //     float y = keyHeight * 0.5;

    //     if (i >= 10) {
    //         y = y + keyHeight + keyPadding * 2;
    //     }

    //     g.translate(x, y);
    //     g.scale(1, 0.5);
    //     g.color(c);
    //     g.tint(c);
    //     g.draw(meshKey);

    //     g.scale(1, 2);

    //     g.tint(1);
    //     fontRender.write(blackkeyLabels[i].c_str(), fontSize);
    //     fontRender.renderAt(g, {keyWidth * 0.5 - 5, keyHeight * 0.1, 0.0});

    //     g.popMatrix();
    // }

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
    if (midiNote > 0 && (!holdNotes || true)) {
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
                            m.controlValue() * 12.f - 6.f);
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
