#pragma once

#include "./material.h"
#include "./hittable.h"

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : index_of_refraction(index_of_refraction) {}

    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        attenuation = color{1.0, 1.0, 1.0};
        double refraction_ratio = rec.front_face ? (1.0 / index_of_refraction) : index_of_refraction;

        vec3 unit_direction = r_in.direction().normalized();
        auto cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0);
        auto sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool total_internal_reflection = refraction_ratio * sin_theta > 1.0;

        vec3 direction;
        if (total_internal_reflection) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray{rec.p, direction};
        return true;
    }

    double index_of_refraction;
};