#pragma once

#include "./texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb/stb_image.h"

class image_texture : public texture {
public:
    const static int bytes_per_pixel = 3;

    image_texture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = std::unique_ptr<unsigned char>{stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel)};

        if (!data.get()) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = 0;
            height = 0;
        }

        bytes_per_scan_line = bytes_per_pixel * width;
    }

    color value(double u, double v, const vec3&) const override {
        if (data == nullptr) {
            return color{0, 1, 1};
        }

        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0); // origin in image coords is top-left

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        if (i >= width) {
            i = width - 1;
        }
        if (j >= height) {
            j = height - 1;
        }

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data.get() + j * bytes_per_scan_line + i * bytes_per_pixel;

        return color{color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]};
    }

private:
    std::unique_ptr<unsigned char> data;
    int width;
    int height;
    int bytes_per_scan_line;
};