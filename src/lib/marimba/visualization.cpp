
#include <kelon/marimba/visualization.hpp>

#include <al/graphics/al_Shapes.hpp>

#include <kelon/marimba/parameter.hpp>
#include <kelon/util.hpp>

namespace kelon {

/// Number of regions to divide the visual into. The bottom division holds the
/// overtone visualization.
const unsigned int VISUAL_DIVISIONS = 8;

MarimbaVisualizer::MarimbaVisualizer(const MarimbaRange *const range)
    : playingRange(range) {}
MarimbaVisualizer::~MarimbaVisualizer() {}

void MarimbaVisualizer::drawNoteVisual(
    al::Graphics &g, const unsigned char midiNote, const float hardness,
    const float amplitude, const float windowWidth, const float windowHeight,
    const bool reverse) const {
    // Offset the note to zero out at the minimum note.
    const float offsetNote = midiNote - playingRange->first;
    /// The range to display.
    const float range = playingRange->second - playingRange->first;

    /// Height of a divided portion of the window.
    const float divisionHeight = windowHeight / VISUAL_DIVISIONS;

    /// Width of the visual.
    const float w = windowWidth / range;
    /// Height of the visual.
    const float h = amplitude * 3.f * divisionHeight * (VISUAL_DIVISIONS - 1);

    const float x = w * (offsetNote - 0.5);

    const float hue = offsetNote / range;
    const float sat = reduceRange(hardness);
    const float val = 1.f;

    g.pushMatrix();
    g.translate(x, divisionHeight - (reverse ? h : 0.f));
    g.scale(w, h);

    g.color(al::Color(al::HSV(hue, sat, val), amplitude * 30));

    g.draw(mesh);
    g.popMatrix();
}

void MarimbaVisualizer::init() {
    // Initialize the visualization for the voice.
    al::addRect(mesh, 0, 0, 1, 1);
}

AdditiveVisualizedMarimba::AdditiveVisualizedMarimba(
    const AdditiveMarimbaParameters *const params,
    const MarimbaRange *const range)
    : AdditiveMarimbaBase(params), MarimbaVisualizer(range) {}

void AdditiveVisualizedMarimba::init() {
    AdditiveMarimbaBase::init();
    MarimbaVisualizer::init();
}

void AdditiveVisualizedMarimba::onProcess(al::Graphics &g) {
    /// Get the MIDI note from the voice ID.
    const unsigned char note = id();

    for (std::size_t i = 0; i < OSCILLATOR_COUNT; i++) {
        drawNoteVisual(
            g,
            freqToMidiNote(midiNoteToFreq(note) *
                           AdditiveMarimbaBase::parameters->harmonics[i]),
            value(MarimbaParameter::Hardness),
            followers[i].value(),
            value(MarimbaParameter::VisualWidth),
            value(MarimbaParameter::VisualHeight), i != 0);
    }
}

}; // namespace kelon
