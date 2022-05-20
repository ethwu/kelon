
#ifndef KELON_MARIMBA_PARAMETER_H
#define KELON_MARIMBA_PARAMETER_H

#include <string>

#include <al/scene/al_PolySynth.hpp>

namespace kelon {

/// A parameter for a marimba `SynthVoice`.
enum class MarimbaParameter {
    /// Scales the volume of the overtones.
    Hardness,
    /// Adjusts the volume of the overtones relative to one another.
    Brightness,

    /// Amplitude.
    Amplitude,
    /// Pan amount.
    Pan,

    /// Attack time.
    AttackTime,
    /// Decay time.
    DecayTime,
    /// Release time.
    ReleaseTime,

    /// The width of the visual.
    VisualWidth,
    /// The height of the visual.
    VisualHeight,
};

/// Get the name of this marimba parameter.
const std::string &name(const MarimbaParameter &p);

/// Create the given parameter on the passed `voice`.
void create(const MarimbaParameter &p, al::SynthVoice &voice,
            const float default_, const float min, const float max);
/**
 * Get the value of this parameter on the given `voice`. Does not modify
 * `voice`.
 */
float value(const MarimbaParameter &p, al::SynthVoice &voice);
/// Set the value of this parameter on the given `voice`.
void value(const MarimbaParameter &p, al::SynthVoice &voice, const float value);

}; // namespace kelon

#endif
