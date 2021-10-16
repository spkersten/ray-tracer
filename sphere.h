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
            get_sphere_uv(outward_normal, rec.u, rec.v);
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

    double pdf_value(const point3& origin, const point3& direction) const override {
        hit_record rec;
        if (!hit(ray{origin, direction}, 0.001, infinity, rec)) {
            return 0;
        }

        auto cos_theta_max = std::sqrt(1 - radius * radius / (center - origin).length_squared());
        auto solid_angle = 2 * pi * (1 - cos_theta_max);
        return 1 / solid_angle;
    }

    vec3 random(const point3& origin) const override {
        vec3 direction = center - origin;
        auto distance2 = direction.length_squared();
        onb uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius, distance2));
    }

    point3 center;
    double radius;
    std::shared_ptr<material> material;

private:
    // p must be of length 1
    static void get_sphere_uv(const point3& p, double& u, double& v) {
        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

    static vec3 random_to_sphere(double radius, double distance2) {
        // the angle of a ray just touching the sphere
        auto cos_theta_max = std::sqrt(1 - radius * radius / distance2);
        auto r1 = random_double();
        auto r2 = random_double();
        auto z = 1 + r2 * (cos_theta_max - 1);
        auto x = std::cos(2 * pi * r1) * std::sqrt(1 - z * z);
        auto y = std::sin(2 * pi * r1) * std::sqrt(1 - z * z);
        return {x, y, z};
    }
};