#pragma once

#include "../scene.h"
#include "../lambertian.h"
#include "../diffuse_light.h"
#include "../xy_rect.h"

void empty_cornell_box(scene& scene, bool with_light = true) {
    hittable_list& objects = scene.world;

    auto red = std::make_shared<lambertian>(color{0.65, 0.05, 0.05});
    auto white = std::make_shared<lambertian>(color{0.73, 0.73, 0.73});
    auto green = std::make_shared<lambertian>(color{0.12, 0.45, 0.15});
    auto light = std::make_shared<diffuse_light>(color{15, 15, 15});

    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green)); // left
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red)); // right
    if (with_light) {
        scene.light = std::make_shared<xz_rect>(213, 343, 227, 332, 554, -1, light);
        objects.add(scene.light);
    }
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white)); // floor
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white)); // ceiling
    objects.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white)); // back
}
