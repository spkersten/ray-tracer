#pragma once

#include <memory>

#include "../scene.h"
#include "../lambertian.h"
#include "../sphere.h"
#include "../image_texture.h"

void earth(scene& scene) {
    scene.cam.lookfrom = point3{13, 3, 3};
    scene.cam.lookat = point3{0, 2, 0};
    scene.cam.vfov = 20.0;

    hittable_list& world = scene.world;

    auto material_ground = std::make_shared<lambertian>(color{0.8, 0.8, 0.8});
    world.add(std::make_shared<sphere>(point3{0, -100, 0}, 100.0, material_ground));
    
    // auto earth_texture = std::make_shared<image_texture>("earth.jpg");
    auto earth_texture = std::make_shared<image_texture>("Blue_Marble_2002.png");
    auto earth_surface = std::make_shared<lambertian>(earth_texture);
    auto globe = std::make_shared<sphere>(point3{0, 2, 0}, 2, earth_surface);
    world.add(globe);
}
