
#ifndef PADAUK_MARIMBA_H
#define PADAUK_MARIMBA_H

#include <map>
#include <string>
#include <tuple>
#include <utility>

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"

#include "al/scene/al_PolySynth.hpp"

namespace padauk {

/**
 * Instrument for mallet percussion.
 */
class Marimba : public al::SynthVoice {
public:
    /// Intended MIDI range, inclusive.
    static const std::pair<char, char> RANGE;

    /// 2-channel panner.
    gam::Pan<> pan;

    // Oscillators.

    gam::Sine<> rootOsc;
    gam::Sine<> fourthOsc;
    gam::Sine<> tenthOsc;

    gam::Env<3> rootEnv;
    gam::Env<3> fourthEnv;
    gam::Env<3> tenthEnv;

    /// Envelope followers for graphics.
    gam::EnvFollow<> rootFollower;
    gam::EnvFollow<> fourthFollower;
    gam::EnvFollow<> tenthFollower;
    /// Graphics mesh.
    al::Mesh mesh;

    /**
     * Initialize a voice. Called once per voice. New voices are created
     * whenever a voice is requested but none are idle.
     */
    void init() override;
    /// Process audio. Removes the voice from the rendering chain when done.
    void onProcess(al::AudioIOData &io) override;
    /// Process graphics.
    void onProcess(al::Graphics &g) override;
    /// Triggered on note start.
    void onTriggerOn() override;
    /// Triggered on note end.
    void onTriggerOff() override;

    /// User-facing internal trigger parameter types.
    enum class Parameter {
        Hardness,
        Brightness,
        MidiNote,
        Amplitude,
        AttackTime,
        DecayTime,
        SustainTime,
        ReleaseTime,
        Pan,
        FirstOvertone,
        SecondOvertone,
        VisualWidth,
        VisualHeight,
    };
    /// Get the identifier of an internal trigger parameter.
    const std::string &identifier(const Parameter &param) const;
    /// Get the value of an internal trigger parameter. Pure.
    const float value(const Parameter &param);
    /// Set the value of an internal trigger parameter.
    void setValue(const Parameter &param, const float value);
    /// Synchronize the internal trigger parameters with another voice. This
    /// does not modify the other voice.
    void sync(Marimba &other);

    /// Set this instrument in marimba mode.
    void marimba();
    /// Set this instrument in xylophone mode.
    void xylophone();

private:
    /// Draw the visual associated with a given note.
    void drawNoteVisual(al::Graphics &g, const float offsetNote,
                        const float adjustedAmplitude, const bool reverse);

    /// Factor to scale hardness by.
    static const float SCALE_HARDNESS;
    /// Factor to reduce overall amplitude by.
    static const float SCALE_AMPLITUDE;

    /// Number of divisions in the window. Controls how much of the screen is
    /// devoted to the fundamental vs the overtones.
    static const float VISUAL_DIVISIONS;

    /// Minimum time for ADSR envelope values.
    static const float MINIMUM_ADSR_TIME;
    /// Maximum time for ADSR envelope values.
    static const float MAXIMUM_ADSR_TIME;
    /// Default value of an internal trigger parameter's default value.
    static const float PARAMETER_DEFAULT_DEFAULT_VALUE;
    /// Default value of an internal trigger parameter's minimum value.
    static const float PARAMETER_DEFAULT_MINIMUM_VALUE;
    /// Default value of an internal trigger parameter's maximum value.
    static const float PARAMETER_DEFAULT_MAXIMUM_VALUE;

    /// Mapping of internal trigger parameter to their identifiers.
    static const std::map<Parameter, std::string> PARAMETER_IDENTIFIERS;

    /// Default values for the internal trigger parameters.
    static const std::tuple<Parameter, float, float, float>
        INTERNAL_TRIGGER_PARAMETER_DEFAULTS[13];
};

}; // namespace padauk

#endif
