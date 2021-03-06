#pragma once

#include "./vec3.h"
#include "./ray.h"
#include "./utils.h"

class camera {
public:
    camera(
        point3 lookfrom,
        point3 lookat,
        vec3 up,
        double vertical_fov, // degrees
        double aspect_ratio,
        double aperture,
        double focus_distance
    ) {
        auto theta = vertical_fov / 180.0 * pi;
        auto h = tan(theta / 2); // half viewport height / viewport distance from camera
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        w = (lookfrom - lookat).normalized();
        u = cross(up, w).normalized();
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_distance * viewport_width * u;
        vertical = focus_distance * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_distance * w;

        lens_radius = aperture / 2;
    }

    ray get_ray(double s, double t) const {
        auto rd = lens_radius * random_in_unit_disk();
        auto offset = u * rd.x() + v * rd.y(); 

        return ray{
            origin + offset,
            lower_left_corner + s * horizontal + t * vertical - origin - offset
        };
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;
};
