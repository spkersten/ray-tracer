#pragma once

#include <memory>

#include "../scene.h"
#include "../translation.h"
#include "../csg.h"
#include "../sphere.h"
#include "../dielectric.h"
#include "./cornell_box.h"

void cornell_box_csg(scene& scene) {
    scene.background = color{0, 0, 0};
    scene.aspect_ratio = 1;
    scene.image_width = 400;
    scene.samples_per_pixel = 400;
    scene.cam.lookfrom = point3{278, 278, -800};
    scene.cam.lookat = point3{278, 278, 0};
    scene.cam.vfov = 40.0;

    hittable_list& objects = scene.world;
    
    empty_cornell_box(scene);

    {
        auto m = std::make_shared<dielectric>(1.5);
        auto ball = std::make_shared<sphere>(
            point3{0, 0, 0},
            120,
            m
        );
        auto cut_out = std::make_shared<sphere>(
            point3{0, 80, -60},
            120,
            m
        );
        objects.add(
            std::make_shared<translate>(
                std::make_shared<fusion>(
                    ball,
                    cut_out
                ),
                vec3{160, 120, 405}
            )
        );
    }

    // {
    //     auto ball = std::make_shared<sphere>(
    //         point3{0, 0, 0},
    //         120,
    //         std::make_shared<lambertian>(color{0.9, 0.8, 0.1})
    //     );
    //     auto cut_out = std::make_shared<sphere>(
    //         point3{120, 0, 0},
    //         120,
    //         std::make_shared<lambertian>(color{0.9, 0.8, 0.1})
    //     );
    //     objects.add(
    //         std::make_shared<translate>(
    //             std::make_shared<intersection>(
    //                 ball,
    //                 cut_out
    //             ),
    //             vec3{410, 260, 300}
    //         )
    //     );
    // }

    // Glass half sphere
    // auto ball = std::make_shared<sphere>(
    //     point3{277, 247, 277},
    //     120,
    //     std::make_shared<dielectric>(1.5)
    // );
    // auto removed = std::make_shared<box>(
    //     point3{5, 247, 5},
    //     point3{550, 550, 550},
    //     std::make_shared<dielectric>(1.5)
    // );
    // objects.add(
    //     std::make_shared<difference>(
    //         ball,
    //         removed
    //     )
    // );
}