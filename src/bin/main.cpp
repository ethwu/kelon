#include "app.hpp"

int main() {
    kelon::App app;

    app.dimensions(1200, 900);

    app.configureAudio(48000., 512, 2, 0);
    app.start();

    return 0;
}
