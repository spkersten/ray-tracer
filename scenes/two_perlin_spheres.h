#pragma once

#include <memory>

#include "../scene.h"
#include "../noise_texture.h"
#include "../lambertian.h"
#include "../sphere.h"

void two_perlin_spheres(scene& scene) {
    scene.cam.lookfrom = point3(13,2,3);
    scene.cam.lookat = point3(0,0,0);
    scene.cam.vfov = 20.0;

    hittable_list& objects = scene.world;

    auto pertext = std::make_shared<turbulence_texture>(4);
    objects.add(std::make_shared<sphere>(
        point3{0, -1000, 0}, 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(
        point3{0, 2, 0}, 2, std::make_shared<lambertian>(pertext)));
}
