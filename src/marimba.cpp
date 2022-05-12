
#include "marimba.hpp"

#include <math.h>

#include "al/graphics/al_Shapes.hpp"
#include "al/types/al_Color.hpp"

#include "util.hpp"

const char C2 = 36;
const char C4 = 60;
const char C7 = 96;

void Marimba::init() {
    // Set the curvature of the ADSR envelope.
    env.curve(0.3);
    // Set ADSR envelope levels.
    env.levels(0, 1, 0.1, 0);
    // Sustain point 2 until release.
    env.sustainPoint(2);

    env2.curve(1);
    env2.levels(0, 1, 0.3, 0);
    env2.sustainPoint(2);

    // Set up the main parameters of the voice.
    for (const auto &values : INTERNAL_TRIGGER_PARAMETER_DEFAULTS) {
        createInternalTriggerParameter(identifier(std::get<0>(values)),

                                       std::get<1>(values), std::get<2>(values),
                                       std::get<3>(values));
    }

    //// Visualization ////

    // Initialize the visualization for the voice.
    al::addRect(mesh, 0, 0, 1, 1);
}

void Marimba::onProcess(al::AudioIOData &io) {
    // Set values according to internal trigger parameter values.

    const float hardness = value(Parameter::Hardness);
    const float freq = value(Parameter::Frequency);
    const unsigned char note = freqToMidiNote(freq);

    sineOsc.freq(freq);
    sineOsc2.freq(freq * 2.0f);

    const float attackTime = std::pow(hardness, 2.0f) / 4096.0f;
    const float releaseTime = (96.0f - float(note)) / 80.0f + 0.3f;
    const float decayTime = attackTime;

    gam::real *adsrLengths = env.lengths();
    adsrLengths[0] = attackTime;
    adsrLengths[1] = decayTime;
    // adsrLengths[2] = value(Parameter::SustainTime);
    adsrLengths[3] = releaseTime;

    env2.lengths()[0] = attackTime;
    env2.lengths()[1] = decayTime / 2.0f;
    env2.lengths()[3] = releaseTime / 3.0f;

    pan.pos(value(Parameter::Pan));

    while (io()) {
        float sine1 = sineOsc() * env();
        float sine2 = sineOsc2() * env2();
        // Generate a sample in mono.
        float sampleLeft = sine1 * sine2 * value(Parameter::Amplitude);

        // Visuals follow generated sample.
        envFollower(sampleLeft);

        float sampleRight;
        // Split the mono sample into stereo.
        pan(sampleLeft, sampleLeft, sampleRight);

        // Send the output samples to their respective channels.
        io.out(0) += sampleLeft;
        io.out(1) += sampleRight;
    }

    if (env.done() && envFollower.value() < 0.001f) {
        // This voice is finished, so we remove it from the rendering chain.
        free();
    }
}

void Marimba::onProcess(al::Graphics &g) {
    const float frequency = value(Parameter::Frequency);
    const float amplitude = value(Parameter::Amplitude);
    // Reverse-engineer the MIDI note that is most likely to match this
    // frequency. Offset it to zero out at the minimum note.
    const float note = float(freqToMidiNote(frequency) - RANGE.first);

    const float range = RANGE.second - RANGE.first;

    /// Width of the window.
    const float windowWidth = float(value(Parameter::VisualWidth));

    /// Width of the visual.
    const float w = windowWidth / range;
    const float hardness = value(Parameter::Hardness);
    /// Height of the visual.
    const float h = value(Parameter::VisualHeight) * amplitude;

    const float x = w * (note - 0.5);

    const float hue = note / range;
    const float sat = hardness / 12.f;
    const float val = 0.9;

    g.pushMatrix();
    g.translate(x, 0);
    g.scale(w, h);

    g.color(al::Color(al::HSV(hue, sat, val), envFollower.value() * 30));

    g.draw(mesh);
    g.popMatrix();
}

void Marimba::onTriggerOn() { env.reset(); }

void Marimba::onTriggerOff() { env.release(); }

const std::string &Marimba::identifier(const Parameter &param) const {
    return PARAMETER_IDENTIFIERS.at(param);
}

const float Marimba::value(const Parameter &param) {
    return getInternalParameterValue(identifier(param));
}

void Marimba::setValue(const Parameter &param, const float value) {
    setInternalParameterValue(identifier(param), value);
}

void Marimba::sync(Marimba &other) {
    for (const auto &param : INTERNAL_TRIGGER_PARAMETER_DEFAULTS) {
        const Parameter &p = std::get<0>(param);
        setValue(p, other.value(p));
    }
}

const std::pair<char, char> Marimba::RANGE = {C2, C7};

const float Marimba::MINIMUM_ADSR_TIME = 0.001;
const float Marimba::MAXIMUM_ADSR_TIME = 3.0;
const float Marimba::PARAMETER_DEFAULT_DEFAULT_VALUE = 0.0;
const float Marimba::PARAMETER_DEFAULT_MINIMUM_VALUE = -9999.0;
const float Marimba::PARAMETER_DEFAULT_MAXIMUM_VALUE = 9999.0;

const std::map<Marimba::Parameter, std::string> Marimba::PARAMETER_IDENTIFIERS =
    {
        {Parameter::Hardness, "hardness"},
        {Parameter::Frequency, "frequency"},
        {Parameter::Amplitude, "amplitude"},
        {Parameter::AttackTime, "attack_time"},
        {Parameter::DecayTime, "decay_time"},
        {Parameter::SustainTime, "sustain_time"},
        {Parameter::ReleaseTime, "release_time"},
        {Parameter::Pan, "pan"},
        {Parameter::VisualWidth, "visual_w"},
        {Parameter::VisualHeight, "visual_h"},
};

const std::tuple<Marimba::Parameter, float, float, float>
    Marimba::INTERNAL_TRIGGER_PARAMETER_DEFAULTS[10] = {
        {Marimba::Parameter::Hardness, 2, 0.8, 12},

        {Marimba::Parameter::Frequency, 440, 27.50, 13289.75},
        {Marimba::Parameter::Amplitude, 0.3, 0.0, 1.0},

        {Marimba::Parameter::AttackTime, 0.01, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},
        {Marimba::Parameter::DecayTime, 0.1, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},
        {Marimba::Parameter::SustainTime, 0.1, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},
        {Marimba::Parameter::ReleaseTime, 0.1, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},

        {Marimba::Parameter::Pan, 0.0, -1.0, 1.0},

        {Marimba::Parameter::VisualWidth,
         Marimba::PARAMETER_DEFAULT_DEFAULT_VALUE,
         Marimba::PARAMETER_DEFAULT_MINIMUM_VALUE,
         Marimba::PARAMETER_DEFAULT_MAXIMUM_VALUE},
        {Marimba::Parameter::VisualHeight,
         Marimba::PARAMETER_DEFAULT_DEFAULT_VALUE,
         Marimba::PARAMETER_DEFAULT_MINIMUM_VALUE,
         Marimba::PARAMETER_DEFAULT_MAXIMUM_VALUE},
};
