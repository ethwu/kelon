
#ifndef MARIMBA_H
#define MARIMBA_H

#include <map>
#include <string>
#include <tuple>
#include <utility>

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"

#include "al/scene/al_PolySynth.hpp"

/**
 * Instrument for mallet percussion.
 */
class Marimba : public al::SynthVoice {
public:
    /// Intended MIDI range, inclusive.
    static const std::pair<char, char> RANGE;

    /// 2-channel panner.
    gam::Pan<> pan;
    /// Sine wave oscillator.
    gam::Sine<> sineOsc;
    gam::Sine<> sineOsc2;
    /// Low frequency oscillator.
    gam::LFO<> lfOsc;
    /// ADSR envelope.
    gam::Env<3> env;
    gam::Env<3> env2;

    /// Envelope follower for graphics.
    gam::EnvFollow<> envFollower;
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
        Note,
        Frequency,
        Amplitude,
        AttackTime,
        DecayTime,
        SustainTime,
        ReleaseTime,
        Pan,
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

private:
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
        INTERNAL_TRIGGER_PARAMETER_DEFAULTS[10];
};

#endif
