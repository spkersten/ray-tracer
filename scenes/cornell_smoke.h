#pragma once

#include <memory>

#include "../scene.h"
#include "../rotation.h"
#include "../constant_medium.h"
#include "../box.h"
#include "../translation.h"
#include "./cornell_box.h"

void cornell_smoke(scene& scene) {
    scene.aspect_ratio = 1.0;
    scene.image_width = 600;
    scene.samples_per_pixel = 1500;
    scene.cam.lookfrom = point3{278, 278, -800};
    scene.cam.lookat = point3{278, 278, 0};
    scene.cam.vfov = 40.0;

    hittable_list& objects = scene.world;
    
    empty_cornell_box(scene, false);

    auto light = std::make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(std::make_shared<xz_rect>(113, 443, 127, 432, 554, light));

    auto white = std::make_shared<lambertian>(color{0.73, 0.73, 0.73});

    auto box1 = std::make_shared<translate>(
        std::make_shared<rotate_y>(
            std::make_shared<box>(point3{0, 0, 0}, point3{165, 165, 165}, white),
            pi / 180 * -18
        ),
        vec3{130, 0, 65}
    );
    objects.add(std::make_shared<constant_medium>(box1, 0.01, color{0, 0, 0}));

    auto box2 = std::make_shared<translate>(
        std::make_shared<rotate_y>(
            std::make_shared<box>(point3{0, 0, 0}, point3{165, 330, 165}, white),
            pi / 180 * 15
        ),
        vec3{265, 0, 295}
    );

    objects.add(std::make_shared<constant_medium>(box2, 0.01, color{1, 1, 1}));
}