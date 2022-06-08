
#include <kelon/marimba/subtractive.hpp>

#include <iostream>

#include <kelon/util.hpp>

namespace kelon {

SubtractiveMarimbaBase::SubtractiveMarimbaBase(
    const SubtractiveMarimbaParameters *const params)
    : al::SynthVoice(), parameters(params) {}

SubtractiveMarimbaBase::~SubtractiveMarimbaBase() {}

void SubtractiveMarimbaBase::init() {
    envelope.curve(SubtractiveMarimbaParameters::ENVELOPE_CURVE);
    envelope.levels(SubtractiveMarimbaParameters::ENVELOPE_LEVELS[0],
                    SubtractiveMarimbaParameters::ENVELOPE_LEVELS[1],
                    SubtractiveMarimbaParameters::ENVELOPE_LEVELS[2],
                    SubtractiveMarimbaParameters::ENVELOPE_LEVELS[3]);

    oscillator.harmonics(12);

    // Set up the main parameters of the voice.
    for (const auto &values : parameters->internalTriggerParameters) {
        create(std::get<0>(values), *this, std::get<1>(values),
               std::get<2>(values), std::get<3>(values));
    }
}

void SubtractiveMarimbaBase::onProcess(al::AudioIOData &io) {
    // Set values according to internal trigger parameter values.

    /// Get the MIDI note we are playing from our voice ID.
    const unsigned char note = id();
    /// Location as a percent distance from C6.
    const float location = 1.f - float(note - C6) / float(C8 - C6);

    std::cout << id() << "\t@\t" << midiNoteToFreq(id()) << std::endl;
    oscillator.freq(midiNoteToFreq(note));
    gam::real *const lengths = envelope.lengths();
    lengths[0] = value(MarimbaParameter::AttackTime, *this);
    lengths[1] = value(MarimbaParameter::DecayTime, *this);
    lengths[2] =
        marimbaDecay(note, value(MarimbaParameter::ReleaseTime, *this));

    // Set parameters on the comb filter.
    comb.set(value(MarimbaParameter::Delay, *this),
             value(MarimbaParameter::Feedforward, *this),
             value(MarimbaParameter::Feedback, *this));
    comb.freq(freqToMidiNote(note));

    // Set the pan.
    pan.pos(value(MarimbaParameter::Pan, *this));

    while (io()) {
        // Generate a sample in mono.

        static constexpr float NOISE_MIX = 0.1;

        /// Sample mixing oscillator output and noise.
        float sampleLeft = oscillator() * (1 - NOISE_MIX) + noise() * NOISE_MIX;

        sampleLeft = comb() * sampleLeft * envelope() *
                     value(MarimbaParameter::Amplitude, *this);

        // Graphics follow the mono sample.
        follower(sampleLeft);

        float sampleRight;
        pan(sampleLeft, sampleLeft, sampleRight);

        io.out(0) += sampleLeft;
        io.out(1) += sampleRight;
    }

    // Wait for the envelope to be finished.
    if (follower.done()) {
        // Free the voice.
        free();
    }
}

void SubtractiveMarimbaBase::onTriggerOn() { envelope.reset(); }

void SubtractiveMarimbaBase::onTriggerOff() {}

}; // namespace kelon
