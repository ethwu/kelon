
#include <kelon/marimba/subtractive.hpp>

namespace kelon {

SubtractiveMarimbaBase::SubtractiveMarimbaBase(
    const SubtractiveMarimbaParameters *const params)
    : al::SynthVoice(), parameters(params) {}

SubtractiveMarimbaBase::~SubtractiveMarimbaBase() {}

void SubtractiveMarimbaBase::init() {}
}; // namespace kelon
