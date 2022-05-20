
#ifndef KELON_MARIMBA_XYLOPHONE_H
#define KELON_MARIMBA_XYLOPHONE_H

#include <kelon/marimba/additive.hpp>
#include <kelon/marimba/visualization.hpp>

namespace kelon {

class AdditiveMarimba : public AdditiveVisualizedMarimba {
public:
    AdditiveMarimba();

    static const std::tuple<MarimbaParameter, float, float, float>
        INTERNAL_TRIGGER_PARAMETERS[INTERNAL_PARAMETER_COUNT];

protected:
    /// The range to visualize.
    static const std::pair<const unsigned char, const unsigned char> RANGE;
};

class AdditiveXylophone : public AdditiveVisualizedMarimba {
public:
    AdditiveXylophone();

    static const std::tuple<MarimbaParameter, float, float, float>
        INTERNAL_TRIGGER_PARAMETERS[INTERNAL_PARAMETER_COUNT];

protected:
    /// The range to visualize.
    static const std::pair<const unsigned char, const unsigned char> RANGE;
};

}; // namespace kelon

#endif
