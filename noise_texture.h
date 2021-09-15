#pragma once

#include "./texture.h"
#include "./perlin.h"

class noise_texture : public texture {
public:
    noise_texture(double scale) : scale(scale) {}

    color value(double u, double v, const point3& p) const override {
        return color{1, 1, 1} * 0.5 * (1.0 + noise.noise(scale * p));
    }

private:
    double scale;
    perlin noise;
};
