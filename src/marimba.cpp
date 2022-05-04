
#include "marimba.hpp"

#include "al/graphics/al_Shapes.hpp"
#include "al/types/al_Color.hpp"

void Marimba::init() {
    // Intialize envelope
    mAmpEnv.curve(0.3); // make segments lines
    mAmpEnv.levels(0, 1, 0.3, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued

    addRect(mMesh, 0, 0, 1, 1);
    // This is a quick way to create parameters for the voice. Trigger
    // parameters are meant to be set only when the voice starts, i.e. they
    // are expected to be constant within a voice instance. (You can actually
    // change them while you are prototyping, but their changes will only be
    // stored and aplied when a note is triggered.)

    createInternalTriggerParameter("hardness", 2, 1, 5);
    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 440, 20, 5000);
    createInternalTriggerParameter("attackTime", 0.01, 0.001, 3.0);
    createInternalTriggerParameter("decayTime", 0.1, 0.001, 3.0);
    createInternalTriggerParameter("decayLevel", 0.6, 0.00, 1.0);
    createInternalTriggerParameter("releaseTime", 0.2, 0.001, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);

    createInternalTriggerParameter("pianoKeyX");
    createInternalTriggerParameter("pianoKeyY");
    createInternalTriggerParameter("pianoKeyWidth");
    createInternalTriggerParameter("pianoKeyHeight");
}

void Marimba::onProcess(al::AudioIOData &io) {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    mOsc.freq(getInternalParameterValue("frequency"));
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[1] = getInternalParameterValue("decayTime");
    mAmpEnv.levels()[1] = getInternalParameterValue("decayLevel");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io()) {
        float s1 = mOsc() * mAmpEnv() * getInternalParameterValue("amplitude");
        float s2;
        mEnvFollow(s1);
        mPan(s1, s1, s2);
        io.out(0) += s1;
        io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (mAmpEnv.done() && (mEnvFollow.value() < 0.001f))
        free();
}

void Marimba::onProcess(al::Graphics &g) {
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");

    float x = getInternalParameterValue("pianoKeyX");
    float y = getInternalParameterValue("pianoKeyY");

    float w = getInternalParameterValue("pianoKeyWidth");
    float h = getInternalParameterValue("pianoKeyHeight");

    float hue = (frequency - 200) / 1000;
    float sat = amplitude;
    float val = 0.9;

    g.pushMatrix();
    g.translate(x, y);
    g.scale(w, h);

    g.color(al::Color(al::HSV(hue, sat, val), mEnvFollow.value() * 30));

    g.draw(mMesh);
    g.popMatrix();
}

void Marimba::onTriggerOn() { mAmpEnv.reset(); }

void Marimba::onTriggerOff() { mAmpEnv.release(); }