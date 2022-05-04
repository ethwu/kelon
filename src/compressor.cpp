
#include "compressor.hpp"

float linearToDecibels(float linear) {
    return 20.f * std::log10(std::abs(linear));
}

float decibelsToLinear(float decibels) {
    return std::pow(10.f, decibels / 20.f);
}

CompressorStats::CompressorStats(float pre_peak, float duck, float post_peak) {
    this->set(pre_peak, duck, post_peak);
};
void CompressorStats::set(float pre_peak, float duck, float post_peak) {
    this->pre_peak = pre_peak;
    this->duck = duck;
    this->post_peak = post_peak;
};

bool approxEqual(float l, float r) { return std::abs(l - r) < 0.0001f; }

bool operator==(const CompressorStats &l, const CompressorStats &r) {
    return approxEqual(l.pre_peak, r.pre_peak) && approxEqual(l.duck, r.duck) &&
           approxEqual(l.post_peak, r.post_peak);
}
