#include "app.hpp"

int main() {
    padauk::App app;

    app.dimensions(1200, 600);

    app.configureAudio(48000., 512, 2, 0);
    app.start();

    return 0;
}
