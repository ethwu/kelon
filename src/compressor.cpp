
#include "compressor.hpp"

const int BLOCK_SIZE = 128;

float linearToDecibels(float linear) {
    return 20.f * std::log10(std::abs(linear));
}

float decibelsToLinear(float decibels) {
    return std::pow(10.f, decibels / 20.f);
}

class CompressorStats {
public:
    float pre_peak;
    float duck;
    float post_peak;
    CompressorStats(float pre_peak, float duck, float post_peak) {
        this->set(pre_peak, duck, post_peak);
    };
    void set(float pre_peak, float duck, float post_peak) {
        this->pre_peak = pre_peak;
        this->duck = duck;
        this->post_peak = post_peak;
    };
};

bool approxEqual(float l, float r) { return std::abs(l - r) < 0.0001f; }

bool operator==(const CompressorStats &l, const CompressorStats &r) {
    return approxEqual(l.pre_peak, r.pre_peak) && approxEqual(l.duck, r.duck) &&
           approxEqual(l.post_peak, r.post_peak);
}

template <int block_size> class CompressorPlugin {
public:
    bool useLookAhead = false;
    bool debug = true;

    CompressorStats previousStats = CompressorStats(0.0f, 1.0f, 0.0f);

    CompressorPlugin() {
        gain.prepare(48000.);
        gain.setThreshold(-5.f);
        gain.setRatio(100.f);
        gain.setKnee(20.f);
        gain.setAttackTime(0.0025f);

        lookahead.setDelayTime(0.005f);
        lookahead.prepare(48000., 2 * block_size);
    };

    AudioIOData &operator()(AudioIOData &io) {
        // TODO get look-ahead working
        io.frame(0);
        for (int i = 0; io() && i < BLOCK_SIZE; i++) {
            sidechain_buf[i] =
                std::max(std::abs(io.out(0)), std::abs(io.out(1)));
        }

        if (useLookAhead) {
            gain.computeGainInDecibelsFromSidechainSignal(sidechain_buf,
                                                          gain_buf, block_size);
        } else {
            gain.computeLinearGainFromSidechainSignal(sidechain_buf, gain_buf,
                                                      block_size);
        }

        if (useLookAhead) {
            lookahead.pushSamples(gain_buf, block_size);
            lookahead.process();
            lookahead.readSamples(look_buf, block_size);

            for (int i = 0; i < block_size; i++) {
                gain_buf[i] = decibelsToLinear(look_buf[i]);
            }
        }

        float pre_peak = 0;
        float duck = 1;
        float post_peak = 0;
        io.frame(0);
        for (int i = 0; io() && i < BLOCK_SIZE; i++) {
            pre_peak = std::max(pre_peak, std::abs(io.out(0)));
            pre_peak = std::max(pre_peak, std::abs(io.out(1)));
            duck = std::min(duck, gain_buf[i]);
            io.out(0) *= gain_buf[i];
            io.out(1) *= gain_buf[i];
            post_peak = std::max(post_peak, std::abs(io.out(0)));
            post_peak = std::max(post_peak, std::abs(io.out(1)));
        }

        if (debug) {
            CompressorStats currentStats(pre_peak, duck, post_peak);
            if (!(currentStats == previousStats)) {
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "pre_peak:  " << std::setw(10)
                          << linearToDecibels(pre_peak) << " dB  "
                          << "compress:  " << std::setw(10)
                          << linearToDecibels(duck) << " dB  "
                          << "post_peak: " << std::setw(10)
                          << linearToDecibels(post_peak) << " dB" << std::endl;
                previousStats = currentStats;
            }
        }

        return io;
    }

private:
    GainReductionComputer gain;
    LookAheadGainReduction lookahead;

    float sidechain_buf[block_size];
    float gain_buf[block_size];
    float look_buf[block_size];
};