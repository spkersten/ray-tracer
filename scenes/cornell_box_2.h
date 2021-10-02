#pragma once

#include <memory>

#include "../scene.h"
#include "../metal.h"
#include "../sphere.h"
#include "../dielectric.h"
#include "../xy_rect.h"
#include "./cornell_box.h"

void cornell_box_2(scene& scene) {
    scene.background = color{0, 0, 0};
    scene.aspect_ratio = 1;
    scene.image_width = 400;
    scene.samples_per_pixel = 100;
    scene.cam.lookfrom = point3{278, 278, -800};
    scene.cam.lookat = point3{278, 278, 0};
    scene.cam.vfov = 40.0;

    hittable_list& objects = scene.world;

    empty_cornell_box(scene);

    auto mirror = std::make_shared<yz_rect>(
        20, 275, 150, 400, 2,
        std::make_shared<metal>(color{1, 1, 1}, 0)
    );
    objects.add(mirror);

    auto metal_ball = std::make_shared<sphere>(
        point3{160, 120, 405},
        120,
        std::make_shared<metal>(color{0.9, 0.9, 0.9}, 0.05)
    );
    objects.add(metal_ball);

    point3 bubble_center{410, 260, 300};
    auto bubble = std::make_shared<sphere>(
        bubble_center,
        120,
        std::make_shared<dielectric>(1.5)
    );
    objects.add(bubble);
    auto bubble_inner = std::make_shared<sphere>(
        bubble_center,
        115,
        std::make_shared<dielectric>(1 / 1.5)
    );
    objects.add(bubble_inner);
}