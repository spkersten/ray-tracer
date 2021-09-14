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
        double aspect_ratio
    ) {
        auto theta = vertical_fov / 180.0 * pi;
        auto h = tan(theta / 2); // half viewport height / viewport distance from camera
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        auto w = (lookfrom - lookat).normalized();
        auto u = cross(up, w).normalized();
        auto v = cross(w, u);

        origin = lookfrom;
        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
    }

    ray get_ray(double u, double v) const {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};
