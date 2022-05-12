
#ifndef MARIMBA_H
#define MARIMBA_H

#include <map>
#include <string>
#include <tuple>

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"

#include "al/scene/al_PolySynth.hpp"

/**
 * Instrument for mallet percussion.
 */
class Marimba : public al::SynthVoice {
public:
    /// 2-channel panner.
    gam::Pan<> pan;
    /// Sine wave oscillator.
    gam::Sine<> osc;
    /// ADSR envelope.
    gam::Env<3> env;

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

private:
    /// Minimum time for ADSR envelope values.
    const float MINIMUM_ADSR_TIME = 0.001;
    /// Maximum time for ADSR envelope values.
    const float MAXIMUM_ADSR_TIME = 0.001;
    /// Default value of an internal trigger parameter's default value.
    const float PARAMETER_DEFAULT_DEFAULT_VALUE = 0.0;
    /// Default value of an internal trigger parameter's minimum value.
    const float PARAMETER_DEFAULT_MINIMUM_VALUE = -9999.0;
    /// Default value of an internal trigger parameter's maximum value.
    const float PARAMETER_DEFAULT_MAXIMUM_VALUE = 9999.0;

    /// User-facing internal trigger parameter types.
    enum class Parameter {
        Hardness,
        Frequency,
        Amplitude,
        AttackTime,
        DecayTime,
        SustainTime,
        ReleaseTime,
        Pan,
        VisualX,
        VisualY,
        VisualWidth,
        VisualHeight,
    };
    /// Mapping of internal trigger parameter to their identifiers.
    std::map<Parameter, std::string> parameterIdentifiers = {
        {Parameter::Hardness, "hardness"},
        {Parameter::Frequency, "frequency"},
        {Parameter::Amplitude, "amplitude"},
        {Parameter::AttackTime, "attack_time"},
        {Parameter::DecayTime, "decay_time"},
        {Parameter::SustainTime, "sustain_time"},
        {Parameter::ReleaseTime, "release_time"},
        {Parameter::Pan, "pan"},
        {Parameter::VisualX, "visual_x"},
        {Parameter::VisualY, "visual_y"},
        {Parameter::VisualWidth, "visual_w"},
        {Parameter::VisualHeight, "visual_h"},
    };

    /// Get the identifier of an internal trigger parameter.
    const std::string &identifier(const Parameter &param) const;
    /// Get the value of an internal trigger parameter. Pure.
    const float value(const Parameter &param);

    /// Default values for the internal trigger parameters.
    const std::tuple<Parameter, float, float, float>
        INTERNAL_TRIGGER_PARAMETER_DEFAULTS[8] = {
            {Parameter::Hardness, 2, 0.8, 12},
            {Parameter::Frequency, 440, 20, 5000},
            {Parameter::Amplitude, 0.3, 0.0, 1.0},
            {Parameter::AttackTime, 0.01, MINIMUM_ADSR_TIME, MAXIMUM_ADSR_TIME},
            {Parameter::DecayTime, 0.1, MINIMUM_ADSR_TIME, MAXIMUM_ADSR_TIME},
            {Parameter::SustainTime, 0.1, MINIMUM_ADSR_TIME, MAXIMUM_ADSR_TIME},
            {Parameter::ReleaseTime, 0.1, MINIMUM_ADSR_TIME, MAXIMUM_ADSR_TIME},
            {Parameter::Pan, 0.0, -1.0, 1.0},
        };
};

#endif
