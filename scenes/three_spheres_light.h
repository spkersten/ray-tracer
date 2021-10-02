#pragma once

#include <memory>

#include "../scene.h"
#include "../dielectric.h"
#include "../diffuse_light.h"
#include "../lambertian.h"
#include "../sphere.h"

void three_spheres_light(scene& scene) {
    scene.background = color{0, 0, 0};
    scene.cam.lookfrom = point3{2, 2, 1};
    scene.cam.lookat = point3{0, 0, -1};
    scene.cam.focus_distance = 1.0;
    scene.cam.aperture = 0.0;
    scene.cam.vfov = 40;

    hittable_list& world = scene.world;

    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.8));
    auto material_left = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_center = std::make_shared<dielectric>(1.5);
    auto material_right = std::make_shared<diffuse_light>(2 * color(0.8, 0.6, 0.2));

    world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
}
