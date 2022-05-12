
#ifndef UTIL_H
#define UTIL_H

/// Get the frequency from a MIDI note.
float midiNoteToFreq(const unsigned char note);

/// Get the MIDI note of a frequency.
unsigned char freqToMidiNote(const float freq);

#endif
