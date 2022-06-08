
#ifndef KELON_MARIMBA_SUBTRACTIVE_H
#define KELON_MARIMBA_SUBTRACTIVE_H

#include <tuple>

#include <Gamma/Analysis.h>
#include <Gamma/Delay.h>
#include <Gamma/Effects.h>
#include <Gamma/Envelope.h>
#include <Gamma/Oscillator.h>
#include <al/scene/al_PolySynth.hpp>

#include <kelon/marimba/parameter.hpp>

namespace kelon {

/// Internal parameter count.

/**
 * Parameters required for adjusting the subtractive marimba.
 */
struct SubtractiveMarimbaParameters {
    /// Internal parameter
    static const std::size_t INTERNAL_PARAMETER_COUNT = 12;
    /// Envelope curve.
    static constexpr float ENVELOPE_CURVE = 0.f;
    /// Envelope levels.
    static constexpr float ENVELOPE_LEVELS[4] = {0.f, 1.f, 0.2f, 0.f};

    /// Default values for the internal trigger parameters.
    const std::tuple<const MarimbaParameter, const float, const float,
                     const float>
        internalTriggerParameters[INTERNAL_PARAMETER_COUNT];
};

class SubtractiveMarimbaBase : public al::SynthVoice {
protected:
    const SubtractiveMarimbaParameters *const parameters;

    /// Construct a new subtractive marimba with the given parameters.
    SubtractiveMarimbaBase(const SubtractiveMarimbaParameters *const params);
    /// Destroy the subtractive marimba.
    ~SubtractiveMarimbaBase();

    /// Oscillator.
    gam::DSF<> oscillator;
    /// Noise generator.
    gam::NoiseWhite<> noise;
    /// Comb filter.
    gam::Comb<> comb;
    /// Envelope.
    gam::Env<3> envelope;
    /// Envelope follower for graphics.
    gam::EnvFollow<> follower;

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
