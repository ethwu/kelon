
#ifndef PADAUK_UTIL_H
#define PADAUK_UTIL_H

namespace padauk {

const char C2 = 36;
const char C4 = 60;
const char C6 = 84;
const char C7 = 96;

/// Get the frequency from a MIDI note.
float midiNoteToFreq(const unsigned char note);

/// Get the MIDI note of a frequency.
unsigned char freqToMidiNote(const float freq);

/// Reduce the range of a float from [0.0, 1.0] to [0.2, 1.0].
float reduceRange(const float f);

}; // namespace padauk

#endif
