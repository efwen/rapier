#pragma once
#include "core/window.hpp"
#include "util/version.hpp"

namespace rp::gfx {
    //Initialize the graphics enginep
    void init(Window* window, const std::string appName, util::Version appVersion, const std::string& engineName, util::Version engineVersion);

    //Shutdown the graphics engine
    void shutdown();

    // Draws the next frame
    void draw();
}
