
#ifndef MARIMBA_H
#define MARIMBA_H

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"

#include "al/scene/al_PolySynth.hpp"

class Marimba : public al::SynthVoice {
public:
    // Unit generators
    gam::Pan<> mPan;
    gam::Sine<> mOsc;
    gam::Env<3> mAmpEnv;
    // envelope follower to connect audio output to graphics
    gam::EnvFollow<> mEnvFollow;

    al::Mesh mMesh;

    // Initialize voice. This function will only be called once per voice when
    // it is created. Voices will be reused if they are idle.
    void init() override;

    // The audio processing function
    void onProcess(al::AudioIOData &io) override;

    // The graphics processing function
    void onProcess(al::Graphics &g) override;

    // The triggering functions just need to tell the envelope to start or
    // release The audio processing function checks when the envelope is done to
    // remove the voice from the processing chain.
    void onTriggerOn() override;

    void onTriggerOff() override;
};

#endif
