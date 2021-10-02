#pragma once

#include "./material.h"
#include "./hittable.h"

class metal : public material {
public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const override {
        auto reflected = reflect(r_in.direction().normalized(), rec.normal);
        srec.pdf = nullptr;
        srec.skip_pdf_ray = ray{rec.p, reflected + fuzz * random_in_unit_sphere()};
        srec.attenuation = albedo;
        return dot(srec.skip_pdf_ray.direction(), rec.normal) > 0;
    }

    color albedo;
    double fuzz;
};