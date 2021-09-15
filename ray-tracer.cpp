#include <iostream>
#include <memory>

#include "./bvh.h"
#include "./vec3.h"
#include "./color.h"
#include "./hittable_list.h"
#include "./sphere.h"
#include "./camera.h"
#include "./lambertian.h"
#include "./metal.h"
#include "./dielectric.h"

color ray_color(const ray& r, const hittable& world, int depth) {
    if (depth <= 0) {
        return color{0, 0, 0};
    }

    hit_record rec;
    if (world.hit(r, 0.001, infinity, rec)) {
        
        // Normals: 
        //return 0.5 * (rec.normal + color{1, 1, 1});

        ray scattered;
        color attenuation;
        if (rec.material->scatter(r, rec, attenuation, scattered) || true) {
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        return color{1, 0, 0};
    }

    auto unit_direction = r.direction().normalized();
    auto s = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - s) * color{1.0, 1.0, 1.0} + s * color{0.5, 0.7, 1.0};
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3{0, -1000, 0}, 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center{a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};

            if ((center - point3{4, 0.2, 0}).length() > 0.9) {
                if (choose_mat < 0.75) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    auto sphere_material = std::make_shared<lambertian>(albedo);
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
    world.add(std::make_shared<sphere>(point3{4, 1, 0}, 1.0, material3));

    return world;
}

int main() {
    for (int i = 0; i < 22; i++)
        random_double();

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 10;
    const int max_depth = 50;

    // World
    hittable_list world = random_scene();

    // auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    // auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    // auto material_left   = std::make_shared<dielectric>(1.5);
    // auto material_right  = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    // world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    // world.add(std::make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    // world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    // world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    bvh_node world_tree{
        world,
        0,
        0,
    };

    // Camera
    point3 lookfrom{13, 2, 3};
    point3 lookat{0, 0, 0};
    vec3 up{0, 1, 0};
    auto focus_distance = 10.0;
    auto aperture = 0.1;
    camera camera{
        lookfrom,
        lookat,
        up,
        20.0,
        aspect_ratio,
        aperture,
        focus_distance
    };

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color;
            for (int s = 0; s < samples_per_pixel; s++) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                auto r = camera.get_ray(u, v);
                pixel_color += ray_color(r, world_tree, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone\n";
}