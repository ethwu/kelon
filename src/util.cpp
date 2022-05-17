
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

/// Reduce the range of a float from [0.0, 1.0] to [0.2, 1.0].
float reduceRange(const float f) { return f * 4.f / 5.f + 0.2f; }

