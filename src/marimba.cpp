
#include "marimba.hpp"

#include "al/graphics/al_Shapes.hpp"
#include "al/types/al_Color.hpp"

#include "util.hpp"

void Marimba::init() {
    // Set the curvature of the ADSR envelope.
    env.curve(0.3);
    // Set ADSR envelope levels.
    env.levels(0, 1, 0.3, 0);
    // Sustain point 2 until release.
    env.sustainPoint(2);

    // Set up the main parameters of the voice.
    for (auto values : INTERNAL_TRIGGER_PARAMETER_DEFAULTS) {
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

    osc.freq(value(Parameter::Frequency));

    gam::real *adsrLengths = env.lengths();
    adsrLengths[0] = value(Parameter::AttackTime);
    adsrLengths[1] = value(Parameter::DecayTime);
    adsrLengths[2] = value(Parameter::SustainTime);
    adsrLengths[3] = value(Parameter::ReleaseTime);

    pan.pos(value(Parameter::Pan));

    while (io()) {
        // Generate a sample in mono.
        float sampleLeft = osc() * env() * value(Parameter::Amplitude);

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
    float frequency = value(Parameter::Frequency);
    float amplitude = value(Parameter::Amplitude);

    float x = value(Parameter::VisualX);
    float y = value(Parameter::VisualY);
    float w = value(Parameter::VisualWidth);
    float h = value(Parameter::VisualHeight);

    float hue = (frequency - 200) / 1000;
    float sat = amplitude;
    float val = 0.9;

    g.pushMatrix();
    g.translate(x, y);
    g.scale(w, h);

    g.color(al::Color(al::HSV(hue, sat, val), envFollower.value() * 30));

    g.draw(mesh);
    g.popMatrix();
}

void Marimba::onTriggerOn() { env.reset(); }

void Marimba::onTriggerOff() { env.release(); }

const std::string &Marimba::identifier(const Parameter &param) const {
    return parameterIdentifiers.at(param);
}

const float Marimba::value(const Parameter &param) {
    return getInternalParameterValue(identifier(param));
}