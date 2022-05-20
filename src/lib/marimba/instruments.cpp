
#include <kelon/marimba/instruments.hpp>

#include <kelon/marimba/additive.hpp>
#include <kelon/util.hpp>

namespace kelon {

/// The minimum time allowed in an ADSR field.
const float MINIMUM_ADSR_TIME = 0.001;
/// The maximum time allowed in an ADSR field.
const float MAXIMUM_ADSR_TIME = 2.0;

/// Constants for the marimba.
const AdditiveMarimbaParameters additiveMarimbaParameters{
    {1, 4, 10},
    4,
    2,
    {
        {MarimbaParameter::Hardness, 0.5, 0.0, 1.0},
        {MarimbaParameter::Brightness, 0.6, 0.0, 1.0},

        {MarimbaParameter::Amplitude, 0.8, 0.0, 1.0},

        {MarimbaParameter::AttackTime, 0.015, MINIMUM_ADSR_TIME,
         MAXIMUM_ADSR_TIME},
        {MarimbaParameter::DecayTime, 0.15, MINIMUM_ADSR_TIME,
         MAXIMUM_ADSR_TIME},
        {MarimbaParameter::ReleaseTime, 1.5, MINIMUM_ADSR_TIME,
         MAXIMUM_ADSR_TIME},

        {MarimbaParameter::Pan, 0.0, -1.0, 1.0},

        {MarimbaParameter::VisualWidth, 1200, 0, 4096},
        {MarimbaParameter::VisualHeight, 600, 0, 4096},
    }};

/// The visualized playing range of the marimba.
const MarimbaRange additiveMarimbaRange = {C2, C8};

/// Constants for the xylophone.
const AdditiveMarimbaParameters additiveXylophoneParameters{
    {1, 3, 6},
    4,
    2,
    {
        {MarimbaParameter::Hardness, 0.75, 0.0, 1.0},
        {MarimbaParameter::Brightness, 0.25, 0.0, 1.0},

        {MarimbaParameter::Amplitude, 0.8, 0.0, 1.0},

        {MarimbaParameter::AttackTime, 0.01, MINIMUM_ADSR_TIME,
         MAXIMUM_ADSR_TIME},
        {MarimbaParameter::DecayTime, 0.15, MINIMUM_ADSR_TIME,
         MAXIMUM_ADSR_TIME},
        {MarimbaParameter::ReleaseTime, 1.5, MINIMUM_ADSR_TIME,
         MAXIMUM_ADSR_TIME},

        {MarimbaParameter::Pan, 0.0, -1.0, 1.0},

        {MarimbaParameter::VisualWidth, 1200, 0, 4096},
        {MarimbaParameter::VisualHeight, 600, 0, 4096},
    }};

/// The visualized playing range of the xylophone.
const MarimbaRange additiveXylophoneRange = {C2, C8};

AdditiveMarimba::AdditiveMarimba()
    : AdditiveVisualizedMarimba(&additiveMarimbaParameters,
                                &additiveMarimbaRange){};

AdditiveXylophone::AdditiveXylophone()
    : AdditiveVisualizedMarimba(&additiveXylophoneParameters,
                                &additiveXylophoneRange){};

}; // namespace kelon
