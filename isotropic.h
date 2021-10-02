#pragma once

#include "./material.h"
#include "./texture.h"
#include "./hittable.h"

class isotropic : public material {
public:
    isotropic(color c) : albedo(std::make_shared<solid_color>(c)) {}
    isotropic(std::shared_ptr<texture> a) : albedo(a) {}

    bool scatter(
        const ray& ray_in, const hit_record& rec, scatter_record& srec
    ) const override {
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf = std::make_shared<sphere_pdf>();
        return true;
    }

    double scattering_pdf(
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const override {
        return 1 / (4 * pi);
    }

public:
    std::shared_ptr<texture> albedo;
};
