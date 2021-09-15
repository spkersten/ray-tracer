#pragma once

#include "./vec3.h"
#include "./hittable.h"
#include "./material.h"
#include "./perlin.h"

class bumpy_sphere : public hittable {
public:
    bumpy_sphere(
        point3 center, double radius, double noise_amplitude, double noise_scale,
        std::shared_ptr<material> material
    ) : center(center), radius(radius), noise_amplitude(noise_amplitude), 
        noise_scale(noise_scale), material(material) {};

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
            vec3 bump{
                perlin_x.noise(noise_scale * rec.p), 
                perlin_y.noise(noise_scale * rec.p), 
                perlin_z.noise(noise_scale * rec.p)};
            vec3 outward_normal = (rec.p - center) / radius + noise_amplitude * bump;
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

    point3 center;
    double radius;
    double noise_amplitude;
    double noise_scale;
    std::shared_ptr<material> material;
    perlin perlin_x;
    perlin perlin_y;
    perlin perlin_z;

private:
    // p must be of length 1
    static void get_sphere_uv(const point3& p, double& u, double& v) {
        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }
};