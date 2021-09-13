#pragma once

#include "./material.h"
#include "./hittable.h"

class metal : public material {
public:
    metal(const color& albedo) : albedo(albedo) {}

    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        auto reflected = reflect(r_in.direction().normalized(), rec.normal);
        scattered = ray{rec.p, reflected};
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }

    color albedo;
};