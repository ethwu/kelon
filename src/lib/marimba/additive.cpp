
#include <kelon/marimba/additive.hpp>

#include <array>
#include <numeric>

#include <kelon/util.hpp>

namespace kelon {

AdditiveMarimbaBase::AdditiveMarimbaBase(
    const AdditiveMarimbaParameters *const params)
    : al::SynthVoice(), parameters(params) {}

AdditiveMarimbaBase::~AdditiveMarimbaBase() {}

void AdditiveMarimbaBase::init() {
    for (auto &env : envelopes) {
        env.curve(AdditiveMarimbaParameters::ENVELOPE_CURVE);
        env.levels(AdditiveMarimbaParameters::ENVELOPE_LEVELS[0],
                   AdditiveMarimbaParameters::ENVELOPE_LEVELS[1],
                   AdditiveMarimbaParameters::ENVELOPE_LEVELS[2],
                   AdditiveMarimbaParameters::ENVELOPE_LEVELS[3]);
    }

    // Set up the main parameters of the voice.
    for (const auto &values : parameters->internalTriggerParameters) {
        create(std::get<0>(values), *this, std::get<1>(values),
               std::get<2>(values), std::get<3>(values));
    }
}

void AdditiveMarimbaBase::onProcess(al::AudioIOData &io) {
    // Set values according to internal trigger parameter values.

    /// Get the MIDI note we are playing from our voice ID.
    const unsigned char note = id();
    const float hardness = value(MarimbaParameter::Hardness);

    const float freq = midiNoteToFreq(note);

    /// Higher values favor the second overtone, while lower values favor the
    /// first.
    const float brightness = value(MarimbaParameter::Brightness) / 48.f;

    /// Location as a percent distance from C6.
    const float location = 1.f - float(note - C6) / float(C8 - C6);

    /// Which harmonics to sound.
    const float harmonics[AdditiveMarimbaParameters::OSCILLATOR_COUNT] = {
        1,
        value(MarimbaParameter::FirstOvertone),
        value(MarimbaParameter::SecondOvertone),
    };
    // const float harmonics[AdditiveMarimbaParameters::OSCILLATOR_COUNT] =
    //     parameters->_harmonics;

    /// Attack time.
    const float attackTime = value(MarimbaParameter::AttackTime);
    /// Decay time.
    const float decayTime = value(MarimbaParameter::DecayTime);
    /// Release time. Modelled linearly using `marimbaDecay`.
    const float releaseTime = std::fmax(
        marimbaDecay(note, value(MarimbaParameter::ReleaseTime)), 0.15f);

    for (std::size_t i = 0; i < AdditiveMarimbaParameters::OSCILLATOR_COUNT;
         i++) {
        oscillators[i].freq(freq * harmonics[i]);
        gam::real *const lengths = envelopes[i].lengths();
        lengths[0] = attackTime / harmonics[i];
        lengths[1] = decayTime / harmonics[i];
        lengths[2] = releaseTime / harmonics[i];
    }

    /// Hardness scaled by 1 / scaleHardness.
    const float scaledHardness = hardness / parameters->scaleHardness;

    // Set the pan.
    pan.pos(value(MarimbaParameter::Pan));

    while (io()) {
        // Generate a sample in mono.

        const std::array<float, AdditiveMarimbaParameters::OSCILLATOR_COUNT>
            samples = {
                float(oscillators[0]() * envelopes[0]()),
                float(oscillators[1]() * envelopes[1]() * scaledHardness *
                      (1 - brightness)),
                float(oscillators[2]() * envelopes[2]() * scaledHardness *
                      brightness * std::fmin(location, 1)),
            };

        // Set the followers after the generated sample components.
        for (std::size_t i = 0; i < AdditiveMarimbaParameters::OSCILLATOR_COUNT;
             i++) {
            followers[i](samples[i]);
        }

        float sampleLeft;
        float sampleRight;

        // Sum the samples and scale them down by `scaleAmplitude`. Split the
        // generated mono sample into left and right.
        pan(std::accumulate(samples.begin(), samples.end(), 0.f) *
                value(MarimbaParameter::Amplitude) / parameters->scaleAmplitude,
            sampleLeft, sampleRight);

        // Send the output samples to their respective channels.
        io.out(0) += sampleLeft;
        io.out(1) += sampleRight;
    }

    // The first sample will always be loudest, so we can just wait for the
    // first envelope to be silent.
    if (followers[0].done()) {
        // Free the voice.
        free();
    }
}

void AdditiveMarimbaBase::onTriggerOn() {
    for (std::size_t i = 0; i < AdditiveMarimbaParameters::OSCILLATOR_COUNT;
         i++) {
        envelopes[i].reset();
    }
}

void AdditiveMarimbaBase::onTriggerOff() {}

float AdditiveMarimbaBase::value(const MarimbaParameter &p) {
    return kelon::value(p, *this);
}

void AdditiveMarimbaBase::value(const MarimbaParameter &p, const float val) {
    kelon::value(p, *this, val);
}

}; // namespace kelon