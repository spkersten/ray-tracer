#pragma once

#include <cmath>

#include "./texture.h"

class spatial_checker_texture : public texture {
public:
    spatial_checker_texture(
        const std::shared_ptr<texture>& _even, 
        const std::shared_ptr<texture>& _odd,
        double frequency
    ) : even(_even), odd(_odd), frequency(frequency) {}
    
    spatial_checker_texture(const color& c1, const color& c2, double frequency)
      : even(std::make_shared<solid_color>(c1)), 
        odd(std::make_shared<solid_color>(c2)),
        frequency(frequency)
    {}

    color value(double u, double v, const point3& p) const override {
        auto x = static_cast<int>(std::abs(std::floor(frequency * p.x()))) % 2;
        auto y = static_cast<int>(std::abs(std::floor(frequency * p.y()))) % 2;
        auto z = static_cast<int>(std::abs(std::floor(frequency * p.z()))) % 2;

        if (x ^ y ^ z) {
            return odd->value(u, v, p);
        } else {
            return even->value(u, v, p);
        }
    }

    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;
    double frequency;
};

class surface_checker_texture : public texture {
public:
     surface_checker_texture(
        const std::shared_ptr<texture>& _even, 
        const std::shared_ptr<texture>& _odd,
        double frequency
    ) : even(_even), odd(_odd), frequency(frequency) {}
    
    surface_checker_texture(const color& c1, const color& c2, double frequency)
      : even(std::make_shared<solid_color>(c1)), 
        odd(std::make_shared<solid_color>(c2)),
        frequency(frequency)
    {}

    color value(double u, double v, const point3& p) const override {
        auto a = static_cast<int>(std::abs(std::floor(frequency * u))) % 2;
        auto b = static_cast<int>(std::abs(std::floor(frequency * v))) % 2;

        if (a ^ b) {
            return odd->value(u, v, p);
        } else {
            return even->value(u, v, p);
        }
    }

    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;
    double frequency;
};
