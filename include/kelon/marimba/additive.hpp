
#ifndef KELON_MARIMBA_ADDITIVE_H
#define KELON_MARIMBA_ADDITIVE_H

#include <tuple>

#include <Gamma/Analysis.h>
#include <Gamma/Effects.h>
#include <Gamma/Envelope.h>
#include <Gamma/Oscillator.h>
#include <al/scene/al_PolySynth.hpp>

#include <kelon/marimba/parameter.hpp>

namespace kelon {

/**
 * Function calculating the decay for a given note on the marimba. According to
 * Thomas D. Rossing in _Science of Percussion Instruments_ (2006):
 * - The decay time for E3 (MIDI note 52) is 1.5 s (with a resonator;
 *   3.2 s without).
 * - The decay time for E6 (MIDI note 88) is 0.4 s (with a resonator;
 *   0.5 s without).
 * This function models the decay linearly using these data:
 *     decay = 1.5 - 11 * (note - 52) / 360
 */
float marimbaDecay(const unsigned char midiNote, const float baseDecay = 1.5f);


/**
 * Parameters required for adjusting the additive marimba. Should not change
 * during execution.
 */
struct AdditiveMarimbaParameters {
    /// Number of oscillators.
    static const std::size_t OSCILLATOR_COUNT = 3;
    /// Internal parameter count.
    static const std::size_t INTERNAL_PARAMETER_COUNT = 11;
    /// Envelope curve.
    static constexpr float ENVELOPE_CURVE = 0.f;
    /// Envelope levels.
    static constexpr float ENVELOPE_LEVELS[4] = {0.f, 1.f, 0.2f, 0.f};

    /// Which harmonics to play. Not used.
    const unsigned int _harmonics[OSCILLATOR_COUNT];

    /// Factor to scale hardness by.
    const float scaleHardness;
    /// Factor to scale amplitude by.
    const float scaleAmplitude;

    /// Default values for the internal trigger parameters.
    const std::tuple<const MarimbaParameter, const float, const float,
                     const float>
        internalTriggerParameters[INTERNAL_PARAMETER_COUNT];
};

/**
 * Additive synthesizer for generating a marimba- or xylophone-like sound.
 *
 * The ID of a given AdditiveMarimbaBase voice is the MIDI note it sounds.
 */
class AdditiveMarimbaBase : public al::SynthVoice {
public:
    /// Get the value of the given internal trigger parameter.
    float value(const MarimbaParameter &p);
    /// Set the value of the given internal trigger parameter.
    void value(const MarimbaParameter &p, const float value);

protected:
    /**
     * Parameters for the additive marimba. Not owned by the instrument.
     */
    const AdditiveMarimbaParameters *const parameters;

    /// Construct a new additive marimba with the given parameters.
    AdditiveMarimbaBase(const AdditiveMarimbaParameters *const params);
    /// Destroy the additive marimba.
    ~AdditiveMarimbaBase();

    /// Oscillators. Each oscillator is paired with its harmonic number.
    gam::Sine<> oscillators[AdditiveMarimbaParameters::OSCILLATOR_COUNT];
    /// Envelopes. Each oscillator is paired with its harmonic number.
    gam::Env<3> envelopes[AdditiveMarimbaParameters::OSCILLATOR_COUNT];
    /// Envelope followers for graphics.
    gam::EnvFollow<> followers[AdditiveMarimbaParameters::OSCILLATOR_COUNT];

    /// 2-channel panner.
    gam::Pan<> pan;

public:
    void init() override; // Triggered once per voice.
    void onProcess(al::AudioIOData &io) override;
    virtual void onProcess(al::Graphics &g) override = 0;
    void onTriggerOn() override;
    void onTriggerOff() override;
};

}; // namespace kelon
#endif
