#pragma once

#include <iostream>

#include "./vec3.h"
#include "./utils.h"

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
    auto scale = 1.0 / samples_per_pixel;

    // sqrt to get a gamma of 2
    auto r = std::sqrt(scale * pixel_color.x());
    auto g = std::sqrt(scale * pixel_color.y());
    auto b = std::sqrt(scale * pixel_color.z());

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0, 0.999)) << '\n';
}
