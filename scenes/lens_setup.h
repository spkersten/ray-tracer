#pragma once

#include "../scene.h"

void lens_setup(scene& scene) {
    scene.background = color{0, 0, 0};
    scene.aspect_ratio = 3.0/2.0;
    scene.image_width = 600;
    scene.samples_per_pixel = 10000;
    scene.cam.lookfrom = point3{-120, 120, 0};
    scene.cam.lookat = point3{60, 0, 0};
    scene.cam.vfov = 24;
    scene.cam.up = vec3{0, 0, 1};

    hittable_list& objects = scene.world;

    auto horizontal_mat = std::make_shared<lambertian>(color{0.9, 0.2, 0.2});
    auto vettical_mat = std::make_shared<lambertian>(color{0.2, 0.9, 0.2});
    // auto horizontal_mat = std::make_shared<diffuse_light>(color{10, 2, 2});
    // auto vettical_mat = std::make_shared<diffuse_light>(color{2, 10, 2});
    objects.add(
        std::make_shared<yz_rect>(
            -10, 10, 0, 2, -2, 
            horizontal_mat
        )
    );
    objects.add(
        std::make_shared<yz_rect>(
            -1, 1, 0, 10, -12, 
            vettical_mat
        )
    );

    auto R = 27.0;
    auto d = 10;
    auto lens_x = 40.0;
    auto glass = std::make_shared<dielectric>(1.5);
    objects.add(
        std::make_shared<intersection>(
            std::make_shared<sphere>(
                point3{lens_x + -R + d/2, 0, 0},
                R,
                glass
            ),
            std::make_shared<sphere>(
                point3{lens_x + R - d/2, 0, 0},
                R,
                glass
            )
        )
    );

    // screen
    objects.add(
        std::make_shared<yz_rect>(
            -90, 90, -90, 90, 120,
            std::make_shared<lambertian>(color{1, 1, 1})
        )
    );

    // backlight
    objects.add(
        std::make_shared<yz_rect>(
            -20, 20, -20, 20, -100, 
            std::make_shared<diffuse_light>(10 * color(0.8, 0.7, 0.5))
        )
    );
}
