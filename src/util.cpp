
#include "util.hpp"

#include <cmath>

/// Get the frequency from a MIDI note.
float midiNoteToFreq(const unsigned char note) {
    return std::pow(2.f, (float(note) - 69.f) / 12.f) * 440.f;
}

/// Get the MIDI note of a frequency.
unsigned char freqToMidiNote(const float freq) {
    return 12 * std::log2(freq / 440.f) + 69;
}