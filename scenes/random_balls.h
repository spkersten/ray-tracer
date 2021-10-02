#pragma once

#include <memory>

#include "../scene.h"
#include "../lambertian.h"
#include "../sphere.h"
#include "../bumpy_sphere.h"
#include "../checker_texture.h"

void random_scene(scene& scene) {
    scene.cam.lookfrom = point3{13, 2, 3};
    scene.cam.lookat = point3{0, 0, 0};
    scene.cam.focus_distance = 10.0;
    scene.cam.aperture = 0.1;
    scene.cam.vfov = 20;

    hittable_list& world = scene.world;

    auto checker = std::make_shared<spatial_checker_texture>(
        color{0.2, 0.3, 0.1}, color{0.9, 0.9, 0.9}, 10);
    auto ground_material = std::make_shared<lambertian>(checker);
    world.add(std::make_shared<sphere>(point3{0, -1000, 0}, 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center{a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};

            if ((center - point3{4, 0.2, 0}).length() > 0.9) {
                if (choose_mat < 0.75) {
                    // diffuse
                    color albedo = color::random() * color::random();;
                    std::shared_ptr<texture> tex;
                    if (choose_mat < 0.075) {
                        tex = std::make_shared<image_texture>("2k_mars.jpg");
                    } else if (choose_mat < 0.15) {
                        tex = std::make_shared<image_texture>("Blue_Marble_2002.png");
                    } else if (choose_mat < 0.225) {
                        tex = std::make_shared<image_texture>("2k_sun.jpg");
                    } else {
                        tex = std::make_shared<solid_color>(albedo);
                    }
                    auto sphere_material = std::make_shared<lambertian>(tex);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.9) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    auto sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    auto sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3{0, 1, 0}, 1.0, material1));

    auto material2 = std::make_shared<lambertian>(color{0.4, 0.2, 0.1});
    world.add(std::make_shared<sphere>(point3{-4, 1, 0}, 1.0, material2));

    auto material3 = std::make_shared<metal>(color{0.7, 0.6, 0.5}, 0.0);
    world.add(std::make_shared<bumpy_sphere>(
        point3{4, 1, 0}, 1.0, 0.2, 2,
        material3
    ));
}