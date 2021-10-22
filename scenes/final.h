#pragma once

#include <memory>

#include "../scene.h"
#include "../box.h"
#include "../diffuse_light.h"
#include "../sphere.h"
#include "../dielectric.h"
#include "../hittable_list.h"
#include "../lambertian.h"
#include "../vec3.h"
#include "../bvh.h"

void final_scene(scene& scene_desc) {
    scene_desc.image_width       = 800;
    scene_desc.aspect_ratio      = 1.0;
    scene_desc.samples_per_pixel = 1000;
    scene_desc.background        = color(0,0,0);

    scene_desc.cam.aperture = 0.0;
    scene_desc.cam.vfov     = 40.0;
    scene_desc.cam.lookfrom = point3(478, 278, -600);
    scene_desc.cam.lookat   = point3(278, 278, 0);

    hittable_list boxes1;
    auto ground = std::make_shared<lambertian>(color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(std::make_shared<box>(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list& world = scene_desc.world;

    world.add(std::make_shared<bvh_node>(boxes1, 0, 1));

    auto light = std::make_shared<diffuse_light>(color(7, 7, 7));
    auto light_obj = std::make_shared<xz_rect>(123, 423, 147, 413, 554, -1, light);
    world.add(light_obj);
    scene_desc.lights.add(light_obj);

    auto center1 = point3(400, 400, 200);
    // auto center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material = std::make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(std::make_shared<sphere>(center1, 50, moving_sphere_material));

    world.add(std::make_shared<sphere>(point3(260, 150, 45), 50, std::make_shared<dielectric>(1.5)));
    world.add(std::make_shared<sphere>(
        point3(0, 150, 145), 50, std::make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = std::make_shared<sphere>(point3(360,150,145), 70, std::make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(std::make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = std::make_shared<sphere>(point3(0,0,0), 5000, std::make_shared<dielectric>(1.5));
    world.add(std::make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    auto emat = std::make_shared<lambertian>(std::make_shared<image_texture>("Blue_Marble_2002.png"));
    world.add(std::make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = std::make_shared<noise_texture>(0.1);
    world.add(std::make_shared<sphere>(point3(220,280,300), 80, std::make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = std::make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<sphere>(point3::random(0,165), 10, white));
    }

    world.add(std::make_shared<translate>(
        std::make_shared<rotate_y>(
            std::make_shared<bvh_node>(boxes2, 0, 1), 15 * pi / 180),
            vec3(-100,270,395)
        )
    );
}

void default_scene(scene& scene_desc) {
    final_scene(scene_desc);
    scene_desc.image_width       = 400;
    scene_desc.samples_per_pixel = 250;
    scene_desc.max_depth         = 4;
}
