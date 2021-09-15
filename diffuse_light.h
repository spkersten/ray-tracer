#pragma once

#include "./material.h"
#include "./texture.h"

class diffuse_light : public material {
public:
    diffuse_light(std::shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}

    bool scatter(const ray&, const hit_record&, color&, ray&) const override {
        return false;
    }

    color emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

public:
    std::shared_ptr<texture> emit;
};
