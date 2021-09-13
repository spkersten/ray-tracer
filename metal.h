#pragma once

#include "./material.h"
#include "./hittable.h"

class metal : public material {
public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        auto reflected = reflect(r_in.direction().normalized(), rec.normal);
        scattered = ray{rec.p, reflected + fuzz * random_in_unit_sphere()};
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }

    color albedo;
    double fuzz;
};