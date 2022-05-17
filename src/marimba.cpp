
#include "marimba.hpp"

#include <math.h>

#include "al/graphics/al_Shapes.hpp"
#include "al/types/al_Color.hpp"

#include "util.hpp"

const char C2 = 36;
const char C4 = 60;
const char C6 = 84;
const char C7 = 96;

void Marimba::init() {
    rootEnv.curve(0);
    fourthEnv.curve(0);
    tenthEnv.curve(0);

    rootEnv.levels(0, 1, 0.3, 0);
    fourthEnv.levels(0, 0.9, 0.3, 0);
    tenthEnv.levels(0, 0.8, 0.3, 0);

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
    const unsigned char note = value(Parameter::MidiNote);
    const float freq = midiNoteToFreq(note);

    const float amplitude = value(Parameter::Amplitude);

    /// Parameter controlling the brightness. Higher values favor the tenth
    /// harmonic, while lower values favor the fourth.
    const float brightness = value(Parameter::Brightness) / 24.f;

    // Location as a percent distance from C6.
    const float location = 1.f - float(note - C6) / float(RANGE.second - C6);

    // The decay time on note E3 (52) is 1.5s
    // The decay time on note E6 (88) is 0.4s
    // Therefore the decay function is d = 1.5f - 11.f * (note - 52.f) / 360.f

    const float attackTime = value(Parameter::AttackTime);
    const float decayTime = value(Parameter::DecayTime);
    const float sustainTime = std::fmax(
        value(Parameter::SustainTime) - 11.f * (note - 52.f) / 360.f, 0.15f);
    const float releaseTime = value(Parameter::ReleaseTime);

    const float first = value(Parameter::FirstOvertone);
    const float second = value(Parameter::SecondOvertone);

    rootOsc.freq(freq);
    fourthOsc.freq(freq * first);
    tenthOsc.freq(freq * second);

    gam::real *rootLengths = rootEnv.lengths();
    gam::real *fourthLengths = fourthEnv.lengths();
    gam::real *tenthLengths = tenthEnv.lengths();

    fourthLengths[0] = (rootLengths[0] = attackTime) / first;
    fourthLengths[1] = (rootLengths[1] = decayTime) / first;
    fourthLengths[2] = (rootLengths[2] = sustainTime) / first;
    fourthLengths[3] = (rootLengths[3] = releaseTime) / first;

    tenthLengths[0] = attackTime / second;
    tenthLengths[1] = decayTime / second;
    tenthLengths[2] = sustainTime / second;
    tenthLengths[3] = releaseTime / second;

    const float scaledHardness = hardness / SCALE_HARDNESS;

    pan.pos(value(Parameter::Pan));

    while (io()) {
        // Generate a sample in mono.
        float fundamental = rootOsc() * rootEnv() / SCALE_AMPLITUDE;

        float fourth = fourthOsc() * fourthEnv() * scaledHardness /
                       SCALE_AMPLITUDE * (1.f - brightness);
        float tenth = tenthOsc() * tenthEnv() * scaledHardness /
                      SCALE_AMPLITUDE * brightness * std::fmin(location, 1.f);

        // Set followers after the components of the sample.
        rootFollower(fundamental);
        fourthFollower(fourth);
        tenthFollower(tenth);

        float sampleLeft = fundamental + fourth + tenth;

        float sampleRight;
        // Split the mono sample into stereo.
        pan(sampleLeft, sampleLeft, sampleRight);

        // Send the output samples to their respective channels.
        io.out(0) += sampleLeft;
        io.out(1) += sampleRight;
    }

    if (rootEnv.done() && rootFollower.value() < 0.001f) {
        // This voice is finished, so we remove it from the rendering chain.
        free();
    }
}

void Marimba::drawNoteVisual(al::Graphics &g, const float offsetNote,
                             const float amplitude, const bool reverse) {
    // The range of the keyboard.
    const float range = RANGE.second - RANGE.first;

    /// Width of the window.
    const float windowWidth = float(value(Parameter::VisualWidth));
    /// Height of a divided portion of the window.
    const float divisionHeight =
        float(value(Parameter::VisualHeight)) / VISUAL_DIVISIONS;

    /// Width of the visual.
    const float w = windowWidth / range;

    /// Height of the visual.
    const float h = amplitude * 3.f * divisionHeight * (VISUAL_DIVISIONS - 1);

    const float x = w * (offsetNote - 0.5);

    const float hue = offsetNote / range;
    const float sat = reduceRange(value(Parameter::Hardness) / 12.f);
    const float val = 1.f;

    g.pushMatrix();
    g.translate(x, divisionHeight - (reverse ? h : 0.f));
    g.scale(w, h);

    g.color(al::Color(al::HSV(hue, sat, val), amplitude * 30));

    g.draw(mesh);
    g.popMatrix();
}

void Marimba::onProcess(al::Graphics &g) {
    // Offset the note to zero out at the minimum note.
    const float offsetNote = value(Parameter::MidiNote) - RANGE.first;

    drawNoteVisual(g, offsetNote, rootFollower.value(), false);
    drawNoteVisual(g,
                   freqToMidiNote(midiNoteToFreq(offsetNote) *
                                  value(Parameter::FirstOvertone)),
                   fourthFollower.value(), true);
    drawNoteVisual(g,
                   freqToMidiNote(midiNoteToFreq(offsetNote) *
                                  value(Parameter::SecondOvertone)),
                   tenthFollower.value(), true);
}

void Marimba::onTriggerOn() {
    rootEnv.reset();
    fourthEnv.reset();
    tenthEnv.reset();
}

void Marimba::onTriggerOff() {
    return;
    rootEnv.release();
    fourthEnv.release();
    tenthEnv.release();
}

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

void Marimba::marimba() {
    setValue(Parameter::AttackTime, 0.015f);
    setValue(Parameter::Hardness, 3.f);
    setValue(Parameter::Brightness, 8.f);
    setValue(Parameter::FirstOvertone, 4.f);
    setValue(Parameter::SecondOvertone, 10.08f);
}

void Marimba::xylophone() {
    setValue(Parameter::AttackTime, 0.01f);
    setValue(Parameter::Hardness, 9.f);
    setValue(Parameter::Brightness, -3.f);
    setValue(Parameter::FirstOvertone, 3.f);
    setValue(Parameter::SecondOvertone, 6.f);
}

const std::pair<char, char> Marimba::RANGE = {C2, C7 + 12};

const float Marimba::SCALE_HARDNESS = 48.f;
const float Marimba::SCALE_AMPLITUDE = 2.f;
const float Marimba::VISUAL_DIVISIONS = 8.f;

const float Marimba::MINIMUM_ADSR_TIME = 0.001;
const float Marimba::MAXIMUM_ADSR_TIME = 2.0;
const float Marimba::PARAMETER_DEFAULT_DEFAULT_VALUE = 0.0;
const float Marimba::PARAMETER_DEFAULT_MINIMUM_VALUE = -9999.0;
const float Marimba::PARAMETER_DEFAULT_MAXIMUM_VALUE = 9999.0;

const std::map<Marimba::Parameter, std::string> Marimba::PARAMETER_IDENTIFIERS =
    {
        {Parameter::Hardness, "hardness"},
        {Parameter::Brightness, "brightness"},
        {Parameter::MidiNote, "midi_note"},
        {Parameter::Amplitude, "amplitude"},
        {Parameter::AttackTime, "attack_time"},
        {Parameter::DecayTime, "decay_time"},
        {Parameter::SustainTime, "sustain_time"},
        {Parameter::ReleaseTime, "release_time"},
        {Parameter::Pan, "pan"},
        {Parameter::FirstOvertone, "first_overtone"},
        {Parameter::SecondOvertone, "second_overtone"},
        {Parameter::VisualWidth, "visual_w"},
        {Parameter::VisualHeight, "visual_h"},
};

const std::tuple<Marimba::Parameter, float, float, float>
    Marimba::INTERNAL_TRIGGER_PARAMETER_DEFAULTS[13] = {
        {Marimba::Parameter::Hardness, 3, 0, 12},
        {Marimba::Parameter::Brightness, 8, 0, 12},

        {Marimba::Parameter::MidiNote, C4, 0, 127},
        {Marimba::Parameter::Amplitude, 0.8, 0.0, 1.0},

        {Marimba::Parameter::AttackTime, 0.015, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},
        {Marimba::Parameter::DecayTime, 0.15, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},
        {Marimba::Parameter::SustainTime, 1.5, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},
        {Marimba::Parameter::ReleaseTime, 0.1, Marimba::MINIMUM_ADSR_TIME,
         Marimba::MAXIMUM_ADSR_TIME},

        {Marimba::Parameter::Pan, 0.0, -1.0, 1.0},

        {Marimba::Parameter::FirstOvertone, 4.f, 3.f, 4.f},
        {Marimba::Parameter::SecondOvertone, 10.08f, 6.f, 10.08f},

        {Marimba::Parameter::VisualWidth,
         Marimba::PARAMETER_DEFAULT_DEFAULT_VALUE, 0, 4096},
        {Marimba::Parameter::VisualHeight,
         Marimba::PARAMETER_DEFAULT_DEFAULT_VALUE, 0, 4096},
};
