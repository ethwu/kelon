
#ifndef KELON_MARIMBA_SUBTRACTIVE_H
#define KELON_MARIMBA_SUBTRACTIVE_H

#include <al/scene/al_PolySynth.hpp>

namespace kelon {

/**
 * Parameters required for adjusting the subtractive marimba.
 */
struct SubtractiveMarimbaParameters {};
class SubtractiveMarimbaBase : public al::SynthVoice {
protected:
    const SubtractiveMarimbaParameters *const parameters;

    /// Construct a new subtractive marimba with the given parameters.
    SubtractiveMarimbaBase(const SubtractiveMarimbaParameters *const params);
    /// Destroy the subtractive marimba.
    ~SubtractiveMarimbaBase();


public:
    void init() override; // Triggered once per voice.
    void onProcess(al::AudioIOData &io) override;
    virtual void onProcess(al::Graphics &g) override = 0;
    void onTriggerOn() override;
    void onTriggerOff() override;
};
}; // namespace kelon

#endif
