#pragma once

#include "../scene.h"
#include "../noise_texture.h"
#include "../lambertian.h"
#include "../sphere.h"
#include "../diffuse_light.h"
#include "../xy_rect.h"

void simple_light(scene& scene) {
    scene.background = color{0, 0, 0};
    scene.cam.lookfrom = point3{23, 3, 6};
    scene.cam.lookat = point3{0, 2, 0};
    scene.cam.vfov = 20.0;

    hittable_list& objects = scene.world;

    auto perlin = std::make_shared<noise_texture>(4);
    objects.add(std::make_shared<sphere>(point3{0, -1000, 0}, 1000, std::make_shared<lambertian>(perlin)));
    objects.add(std::make_shared<sphere>(point3{0, 2, 0}, 2, std::make_shared<lambertian>(perlin)));

    auto light = std::make_shared<diffuse_light>(color{4, 4, 4});
    objects.add(std::make_shared<xy_rect>(3, 5, 1, 3, -2, light));
}
