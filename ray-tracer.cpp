#include <memory>

#include "./vec3.h"
#include "./color.h"
#include "./checker_texture.h"
#include "./hittable_list.h"
#include "./sphere.h"
#include "./bumpy_sphere.h"
#include "./lambertian.h"
#include "./metal.h"
#include "./dielectric.h"
#include "./image_texture.h"
#include "./noise_texture.h"
#include "./diffuse_light.h"
#include "./xy_rect.h"
#include "./box.h"
#include "./rotation.h"
#include "./translation.h"
#include "./constant_medium.h"
#include "./csg.h"
#include "./scene.h"

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

void three_spheres(scene& scene) {
    scene.cam.lookfrom = point3{-2, 2, 1};
    scene.cam.lookat = point3{0, 0, -1};
    scene.cam.focus_distance = 1.0;
    scene.cam.aperture = 0.0;
    scene.cam.vfov = 20;

    hittable_list& world = scene.world;

    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<dielectric>(1.5);
    auto material_left2   = std::make_shared<dielectric>(1.0 / 1.5);
    auto material_right  = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),  0.4, material_left2));
    world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
}

void three_spheres_light(scene& scene) {
    scene.background = color{0, 0, 0};
    scene.cam.lookfrom = point3{2, 2, 1};
    scene.cam.lookat = point3{0, 0, -1};
    scene.cam.focus_distance = 1.0;
    scene.cam.aperture = 0.0;
    scene.cam.vfov = 40;

    hittable_list& world = scene.world;

    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.8));
    auto material_left = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_center  = std::make_shared<dielectric>(1.5);
    auto material_right  = std::make_shared<diffuse_light>(2 * color(0.8, 0.6, 0.2));

    world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
}

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

void two_perlin_spheres(scene& scene) {
    scene.cam.lookfrom = point3(13,2,3);
    scene.cam.lookat = point3(0,0,0);
    scene.cam.vfov = 20.0;

    hittable_list& objects = scene.world;

    auto pertext = std::make_shared<turbulence_texture>(4);
    objects.add(std::make_shared<sphere>(
        point3{0, -1000, 0}, 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(
        point3{0, 2, 0}, 2, std::make_shared<lambertian>(pertext)));
}

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

void empty_cornell_box(scene& scene, bool with_light = true) {
    hittable_list& objects = scene.world;

    auto red = std::make_shared<lambertian>(color{0.65, 0.05, 0.05});
    auto white = std::make_shared<lambertian>(color{0.73, 0.73, 0.73});
    auto green = std::make_shared<lambertian>(color{0.12, 0.45, 0.15});
    auto light = std::make_shared<diffuse_light>(color{15, 15, 15});

    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green)); // left
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red)); // right
    if (with_light) {
        objects.add(std::make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    }
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white)); // floor
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white)); // ceiling
    objects.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white)); // back
}

void cornell_box(scene& scene) {
    hittable_list& objects = scene.world;

    empty_cornell_box(scene);

    auto white = std::make_shared<lambertian>(color{0.73, 0.73, 0.73});

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
            std::make_shared<box>(point3{0, 0, 0}, point3{165, 330, 165}, white),
            pi / 180 * 15
        ),
        vec3{265, 0, 295}
    );

    objects.add(box2);
}

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

int main() {
    scene scene;

    scene.aspect_ratio = 3.0 / 2.0;
    scene.image_width = 400;
    scene.samples_per_pixel = 400;

    switch (6) {
    case 0:
        three_spheres(scene);
        break;
    case 1:
        random_scene(scene);
        break;
    case 3:
        two_perlin_spheres(scene);
        break;
    case 4:
        earth(scene);
        break;
    case 51:
        three_spheres_light(scene);
        break;
    case 5:
        simple_light(scene);
        break;
    case 6:
        cornell_box_2(scene);
        break;
    case 7:
        cornell_smoke(scene);
        break;
    case 8:
        cornell_box_csg(scene);
        break;
    case 9:
        lens_setup(scene);
    }

    scene.render();
}
