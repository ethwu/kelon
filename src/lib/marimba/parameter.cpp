
#include <kelon/marimba/parameter.hpp>

#include <map>

namespace kelon {

/// Mapping of internal trigger parameter to their identifiers.
const std::map<MarimbaParameter, std::string> PARAMETER_NAMES = {
    {MarimbaParameter::Hardness, "hardness"},
    {MarimbaParameter::Brightness, "brightness"},

    {MarimbaParameter::Amplitude, "amplitude"},
    {MarimbaParameter::Pan, "pan"},

    {MarimbaParameter::AttackTime, "attack_time"},
    {MarimbaParameter::DecayTime, "decay_time"},
    {MarimbaParameter::ReleaseTime, "release_time"},

    {MarimbaParameter::VisualWidth, "visual_w"},
    {MarimbaParameter::VisualHeight, "visual_h"},
};

const std::string &name(const MarimbaParameter &p) {
    return PARAMETER_NAMES.at(p);
}

void create(const MarimbaParameter &p, al::SynthVoice &voice,
            const float default_, const float min, const float max) {
    voice.createInternalTriggerParameter(name(p), default_, min, max);
}

float value(const MarimbaParameter &p, al::SynthVoice &voice) {
    return voice.getInternalParameterValue(name(p));
}

void value(const MarimbaParameter &p, al::SynthVoice &voice,
           const float value) {
    voice.setInternalParameterValue(name(p), value);
}

}; // namespace kelon