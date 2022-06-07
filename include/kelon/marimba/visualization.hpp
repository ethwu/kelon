
#ifndef KELON_MARIMBA_VISUALIZATION_H
#define KELON_MARIMBA_VISUALIZATION_H

#include <utility>

#include <al/graphics/al_Shapes.hpp>

#include <kelon/marimba/additive.hpp>

namespace kelon {

/// Structure representing the range of a marimba.
using MarimbaRange = std::pair<const unsigned char, const unsigned char>;

/// Visualizer for the marimba.
class MarimbaVisualizer {
protected:
    MarimbaVisualizer(const MarimbaRange *const);
    ~MarimbaVisualizer();

    /// Draw the visual associated with a given note.
    void drawNoteVisual(al::Graphics &g, const unsigned char midiNote,
                        const float hardness, const float amplitude,
                        const float windowWidth, const float windowHeight,
                        const bool reverse) const;

private:
    /// Graphics mesh.
    al::Mesh mesh;

public:
    /// The range to visualize. Not owned by the instrument.
    const MarimbaRange * const playingRange;
    /// Initialize this visualizer for a new voice.
    void init();
};

/// ABC for visualizing additive marimbas.
class AdditiveVisualizedMarimba : public AdditiveMarimbaBase,
                                  protected MarimbaVisualizer {
public:
    AdditiveVisualizedMarimba(const AdditiveMarimbaParameters *const params,
                              const MarimbaRange *const range);

    void init() override;
    void onProcess(al::Graphics &g) override;
};

}; // namespace kelon

#endif