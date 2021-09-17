#pragma once

#include "./material.h"
#include "./texture.h"
#include "./hittable.h"

class isotropic : public material {
public:
    isotropic(color c) : albedo(std::make_shared<solid_color>(c)) {}
    isotropic(std::shared_ptr<texture> a) : albedo(a) {}

    bool scatter(
        const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        scattered = ray{rec.p, random_in_unit_sphere()};
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    std::shared_ptr<texture> albedo;
};
