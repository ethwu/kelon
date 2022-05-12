
#include "util.hpp"

#include <cmath>

/// Get the frequency from a MIDI note.
float midiNoteToFreq(const unsigned char note) {
    return std::pow(2.f, (float(note) - 69.f) / 12.f) * 432.f;
}

/// Get the MIDI note of a frequency.
unsigned char freqToMidiNote(const float freq) {
    return std::round(std::log2(freq / 432.f) * 12.f + 69.f);
}