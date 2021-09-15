#pragma once

#include "./vec3.h"
#include "./hittable.h"
#include "./material.h"

class sphere : public hittable {
public:
    sphere() {}
    sphere(point3 center, double radius, std::shared_ptr<material> material)
      : center(center), radius(radius), material(material) {};

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        const vec3 oc = r.origin() - center;
        const auto a = dot(r.direction(), r.direction());
        const auto b = 2.0 * dot(oc, r.direction());
        const auto c = dot(oc, oc) - radius*radius;
        const auto discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            return false;
        } else {
            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (-b - sqrtd) / a / 2;
            if (root < t_min || root > t_max) {
                root = (- b + sqrtd) / a / 2;
                if (root < t_min || root > t_max) {
                    return false;
                }
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.material = material;

            return true;
        }
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = aabb{
            center - vec3{radius, radius, radius},
            center + vec3{radius, radius, radius}
        };
        return true;
    }

    point3 center;
    double radius;
    std::shared_ptr<material> material;
};