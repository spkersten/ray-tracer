#pragma once

#include "./material.h"
#include "./hittable.h"
#include "./texture.h"

class lambertian : public material {
public:
    lambertian(const color& albedo) : albedo(std::make_shared<solid_color>(albedo)) {}
    lambertian(std::shared_ptr<texture> texture) : albedo(texture) {}

    bool scatter(
        const ray&, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }
        scattered = ray{rec.p, scatter_direction};
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    std::shared_ptr<texture> albedo;
};
