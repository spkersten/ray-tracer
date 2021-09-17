#include <iostream>
#include <memory>
#include <optional>
#include <chrono>
#include <iomanip>

#include "./bvh.h"
#include "./vec3.h"
#include "./color.h"
#include "./checker_texture.h"
#include "./hittable_list.h"
#include "./sphere.h"
#include "./bumpy_sphere.h"
#include "./camera.h"
#include "./lambertian.h"
#include "./metal.h"
#include "./dielectric.h"
#include "./image_texture.h"
#include "./noise_texture.h"
#include "./diffuse_light.h"
#include "./xy_rect.h"
#include "./box.h"
#include "./thread_pool.h"
#include "./rotation.h"
#include "./translation.h"

color ray_color(
    const ray& r, const hittable& world, int depth, 
    std::optional<color> background = std::nullopt
) {
    if (depth <= 0) {
        return color{0, 0, 0};
    }

    hit_record rec;
    if (world.hit(r, 0.001, infinity, rec)) {
        
        // Normals: 
        // return 0.5 * (rec.normal + color{1, 1, 1});

        ray scattered;
        color attenuation;
        color emitted = rec.material->emitted(rec.u, rec.v, rec.p);

        if (rec.material->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * ray_color(scattered, world, depth - 1, background);
        } else {
            return emitted;
        }
    } else {
        if (background.has_value()) {
            return background.value();
        } else {
            // Sky box:
            auto unit_direction = r.direction().normalized();
            auto s = 0.5 * (unit_direction.y() + 1.0);
            return (1.0 - s) * color{1.0, 1.0, 1.0} + s * color{0.5, 0.7, 1.0};
        }
    }
}

hittable_list random_scene() {
    hittable_list world;

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

    return world;
}

hittable_list three_spheres() {
    hittable_list world;

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

    return world;
}

hittable_list three_spheres_light() {
    hittable_list world;

    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.8));
    auto material_left = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_center  = std::make_shared<dielectric>(1.5);
    auto material_right  = std::make_shared<diffuse_light>(2 * color(0.8, 0.6, 0.2));

    world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    return world;
}

hittable_list earth() {
    hittable_list world;

    auto material_ground = std::make_shared<lambertian>(color{0.8, 0.8, 0.8});
    world.add(std::make_shared<sphere>(point3{0, -100, 0}, 100.0, material_ground));
    
    // auto earth_texture = std::make_shared<image_texture>("earth.jpg");
    auto earth_texture = std::make_shared<image_texture>("Blue_Marble_2002.png");
    auto earth_surface = std::make_shared<lambertian>(earth_texture);
    auto globe = std::make_shared<sphere>(point3{0, 2, 0}, 2, earth_surface);
    world.add(globe);

    return world;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = std::make_shared<turbulence_texture>(4);
    objects.add(std::make_shared<sphere>(
        point3{0, -1000, 0}, 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(
        point3{0, 2, 0}, 2, std::make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list simple_light() {
    hittable_list objects;

    auto perlin = std::make_shared<noise_texture>(4);
    objects.add(std::make_shared<sphere>(point3{0, -1000, 0}, 1000, std::make_shared<lambertian>(perlin)));
    objects.add(std::make_shared<sphere>(point3{0, 2, 0}, 2, std::make_shared<lambertian>(perlin)));

    auto light = std::make_shared<diffuse_light>(color{4, 4, 4});
    objects.add(std::make_shared<xy_rect>(3, 5, 1, 3, -2, light));

    return objects;
}

hittable_list empty_cornell_box() {
    hittable_list objects;

    auto red = std::make_shared<lambertian>(color{0.65, 0.05, 0.05});
    auto white = std::make_shared<lambertian>(color{0.73, 0.73, 0.73});
    auto green = std::make_shared<lambertian>(color{0.12, 0.45, 0.15});
    auto light = std::make_shared<diffuse_light>(color{15, 15, 15});

    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green)); // left
    objects.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red)); // right
    objects.add(std::make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white)); // floor
    objects.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white)); // ceiling
    objects.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white)); // back

    return objects;
}

hittable_list cornell_box() {
    hittable_list objects = empty_cornell_box();

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

    return objects;
}

hittable_list cornell_box_2() {
    hittable_list objects = empty_cornell_box();

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

    return objects;
}

int main() {
    for (int i = 0; i < 22; i++)
        random_double();

    // Image
    auto aspect_ratio = 3.0 / 2.0;
    int image_width = 400;
    int samples_per_pixel = 400;
    const int max_depth = 50;

    // Camera & World
    point3 lookfrom{13, 2, 3};
    point3 lookat{0, 0, 0};
    vec3 up{0, 1, 0};
    double focus_distance = 10.0;
    double aperture = 0.0;
    double vfov = 20;

    std::optional<color> background;

    hittable_list world;

    switch (6) {
    case 0:
        lookfrom = point3{-2, 2, 1};
        lookat = point3{0, 0, -1};
        focus_distance = 1.0;
        aperture = 0.0;
        vfov = 20;
        world = three_spheres();
        break;
    case 1:
        lookfrom = point3{13, 2, 3};
        lookat = point3{0, 0, 0};
        focus_distance = 10.0;
        aperture = 0.1;
        vfov = 20;
        world = random_scene();
        break;
    case 3:
        world = two_perlin_spheres();
        lookfrom = point3(13,2,3);
        lookat = point3(0,0,0);
        vfov = 20.0;
        break;
    case 4:
        lookfrom = point3{13, 3, 3};
        lookat = point3{0, 2, 0};
        vfov = 20.0;
        world = earth();
        break;
    case 51:
        background = color{0, 0, 0};
        lookfrom = point3{2, 2, 1};
        lookat = point3{0, 0, -1};
        focus_distance = 1.0;
        aperture = 0.0;
        vfov = 40;
        world = three_spheres_light();
        break;
    case 5:
        background = color{0, 0, 0};
        lookfrom = point3{23, 3, 6};
        lookat = point3{0, 2, 0};
        vfov = 20.0;
        world = simple_light();
        break;
    case 6:
        background = color{0, 0, 0};
        aspect_ratio = 1;
        image_width = 400;
        samples_per_pixel = 1000;
        lookfrom = point3{278, 278, -800};
        lookat = point3{278, 278, 0};
        vfov = 40.0;
        world = cornell_box_2();
        break;
    }

    bvh_node world_tree{
        world,
        0,
        0,
    };

    camera camera{
        lookfrom,
        lookat,
        up,
        vfov,
        aspect_ratio,
        aperture,
        focus_distance
    };

    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // Generate image

    std::vector<color> pixel_colors{
        static_cast<size_t>(image_width * image_height), 
        color{0, 0, 0}
    };

    std::vector<int> scanlines;
    for (int j = 0; j < image_height; j++) {
        scanlines.push_back(j);
    }
    // Shuffle scan lines to make estimating time left more accurate, as some part of the
    // image might be slower to trace than other parts.
    std::mt19937 g;
    std::shuffle(scanlines.begin(), scanlines.end(), g);

    auto start = std::chrono::system_clock::now();

    pool<int> p{
        scanlines,
        [&] (int j) {
            for (int i = 0; i < image_width; ++i) {
                color pixel_color;
                for (int s = 0; s < samples_per_pixel; s++) {
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);
                    auto r = camera.get_ray(u, v);
                    pixel_color += ray_color(r, world_tree, max_depth, background);
                }
                pixel_colors[j * image_width + i] = pixel_color;
            }
        },
        [&] (auto lines_left) {
            std::cerr << "\rScanlines remaining: " << lines_left << " / " << image_height;

            auto now = std::chrono::system_clock::now();
            auto time_spend = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
            auto seconds_spend = time_spend % 60;
            auto minutes_spend = time_spend / 60;
            auto hours_spend = time_spend / (60 * 60);

            std::cerr << std::fixed << std::setprecision(2) << std::setfill('0');
            std::cerr << " -- Time spend: " << hours_spend
                     << ":" << std::setw(2) << minutes_spend
                     << ":" << std::setw(2) << seconds_spend;

            auto time_left = static_cast<int>(1.0 * lines_left / (image_height - lines_left) * time_spend);
            auto seconds_left = time_left % 60;
            auto minutes_left = time_left / 60;
            auto hours_left = time_left / (60 * 60);

            std::cerr << " -- Estimated time left: " << hours_left
                              << ":" << std::setw(2) << minutes_left
                              << ":" << std::setw(2) << seconds_left;

            // Some extra white space to account for previous lines that where longer
            std::cerr << "         " << std::flush;
        }
    };

    p.run(4);

    // Write the image

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {            
            write_color(std::cout, pixel_colors[j * image_width + i], samples_per_pixel);
        }
    }

    std::cerr << "\nDone\n";
}