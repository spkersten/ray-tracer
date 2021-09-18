#pragma once

#include <memory>

#include "./aabb.h"
#include "./ray.h"

class material;

struct hit_record {
    point3 p;
    vec3 normal; // points "against" the ray: dot(normal, ray.direction()) < 0
    std::shared_ptr<material> material;
    double t;
    double u;
    double v;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;

    virtual ~hittable() {}
};
