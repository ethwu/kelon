
#ifndef KELON_UTIL_H
#define KELON_UTIL_H

namespace kelon {

const unsigned char C2 = 36;
const unsigned char C4 = 60;
const unsigned char C6 = 84;
const unsigned char C7 = 96;
const unsigned char C8 = 108;

/// Get the frequency from a MIDI note.
float midiNoteToFreq(const unsigned char note);

/// Get the MIDI note of a frequency.
unsigned char freqToMidiNote(const float freq);

/// Reduce the range of a float from [0.0, 1.0] to [0.2, 1.0].
float reduceRange(const float f);

}; // namespace kelon

#endif
