#pragma once

#include "./hittable.h"

class translate : public hittable {
public:
    translate(std::shared_ptr<hittable> p, const vec3& displacement)
      : child(p), offset(displacement) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        ray moved_r{r.origin() - offset, r.direction()};
        if (!child->hit(moved_r, t_min, t_max, rec)) {
            return false;
        } else {
            rec.p += offset;
            return true;
        }
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        aabb temp_box;

        if (!child->bounding_box(time0, time1, temp_box)) {
            return false;
        } else {
            output_box = aabb{
                temp_box.min() + offset,
                temp_box.max() + offset,
            };
            return true;
        }
    }

    std::shared_ptr<hittable> child;
    vec3 offset;    
};
