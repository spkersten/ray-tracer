#pragma once

#include <cmath>

#include "./hittable.h"

class rotate_y : public hittable {
public:
    rotate_y(std::shared_ptr<hittable> p, double angle) 
      : child(p), rot(angle), rot_inverse(rot.inverse()) 
    {
        aabb child_box;
        has_aabb = child->bounding_box(0, 1, child_box);

        point3 min{-infinity, -infinity, -infinity};
        point3 max{infinity, infinity, infinity};

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    vec3 p{
                        i * child_box.min().x() + (1 - i) * child_box.max().x(),
                        j * child_box.min().y() + (1 - j) * child_box.max().y(),
                        k * child_box.min().z() + (1 - k) * child_box.max().z()};

                    vec3 new_v = rot.y_rotated(p);
                        
                    for (int c = 0; c < 3; c++) {
                        min[c] = std::min(min[c], new_v[c]);
                        max[c] = std::max(max[c], new_v[c]);
                    }
                }
            }
        }

        box = aabb{min, max};
    }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        auto origin = rot_inverse.y_rotated(r.origin());
        auto direction = rot_inverse.y_rotated(r.direction());

        ray rotated_r{origin, direction};

        if (child->hit(rotated_r, t_min, t_max, rec)) {
            rec.p = rot.y_rotated(rec.p);
            rec.normal = rot.y_rotated(rec.normal);
            return true;
        } else {
            return false;
        }
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = box;
        return has_aabb;
    }

public:
    std::shared_ptr<hittable> child;
    rotation rot;
    rotation rot_inverse;
    bool has_aabb;
    aabb box;
};
