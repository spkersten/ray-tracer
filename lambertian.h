#pragma once

#include "./material.h"
#include "./hittable.h"
#include "./texture.h"
#include "./onb.h"

class lambertian : public material {
public:
    lambertian(const color& albedo) : albedo(std::make_shared<solid_color>(albedo)) {}
    lambertian(std::shared_ptr<texture> texture) : albedo(texture) {}

    bool scatter(
        const ray&, const hit_record& rec, scatter_record& srec
    ) const override {
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf = std::make_shared<cosine_pdf>(rec.normal);
        return true;
    }

    double scattering_pdf(
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const override {
        auto cosine = dot(rec.normal, scattered.direction().normalized());
        return cosine < 0 ? 0 : cosine / pi;
    }

    std::shared_ptr<texture> albedo;
};
