#pragma once

#include <memory>

#include "../scene.h"
#include "../lambertian.h"
#include "../rotation.h"
#include "../box.h"
#include "../translation.h"
#include "./cornell_box.h"

void cornell_box(scene& scene) {
    scene.background = color{0, 0, 0};
    scene.aspect_ratio = 1;
    scene.image_width = 400;
    scene.samples_per_pixel = 100;
    scene.cam.lookfrom = point3{278, 278, -800};
    scene.cam.lookat = point3{278, 278, 0};
    scene.cam.vfov = 40.0;

    hittable_list& objects = scene.world;

    empty_cornell_box(scene);

    auto white = std::make_shared<lambertian>(color{0.73, 0.73, 0.73});
    auto aluminum = std::make_shared<metal>(color{0.8, 0.85, 0.88}, 0.0);

    auto box1 = std::make_shared<translate>(
        std::make_shared<rotate_y>(
            std::make_shared<box>(point3{0, 0, 0}, point3{165, 165, 165}, white),
            pi / 180 * -18
        ),
        vec3{130, 0, 65}
    );
    objects.add(box1);

    auto box2 = std::make_shared<translate>(
        std::make_shared<rotate_y>(
            std::make_shared<box>(point3{0, 0, 0}, point3{165, 330, 165}, aluminum),
            pi / 180 * 15
        ),
        vec3{265, 0, 295}
    );

    objects.add(box2);
}