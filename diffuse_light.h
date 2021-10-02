#pragma once

#include "./material.h"
#include "./texture.h"

class diffuse_light : public material {
public:
    diffuse_light(std::shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}

    bool scatter(const ray&, const hit_record&, scatter_record&) const override {
        return false;
    }

    color emitted(const ray& r_in, const hit_record& rec) const override {
        if (rec.front_face) {
            return emit->value(rec.u, rec.v, rec.p);
        } else {
            return {0, 0, 0};
        }
    }

public:
    std::shared_ptr<texture> emit;
};
