
#ifndef UTIL_H
#define UTIL_H

/// Get the frequency from a MIDI note.
float midiNoteToFreq(const unsigned char note);

/// Get the MIDI note of a frequency.
unsigned char freqToMidiNote(const float freq);

/// Reduce the range of a float from [0.0, 1.0] to [0.2, 1.0].
float reduceRange(const float f);


#endif
