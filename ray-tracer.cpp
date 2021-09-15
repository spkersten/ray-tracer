#include <iostream>
#include <memory>

#include "./bvh.h"
#include "./vec3.h"
#include "./color.h"
#include "./checker_texture.h"
#include "./hittable_list.h"
#include "./sphere.h"
#include "./camera.h"
#include "./lambertian.h"
#include "./metal.h"
#include "./dielectric.h"
#include "./image_texture.h"
#include "./noise_texture.h"

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
        if (rec.material->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        return color{0, 0, 0};
    }

    auto unit_direction = r.direction().normalized();
    auto s = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - s) * color{1.0, 1.0, 1.0} + s * color{0.5, 0.7, 1.0};
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
    world.add(std::make_shared<sphere>(point3{4, 1, 0}, 1.0, material3));

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

    auto pertext = std::make_shared<noise_texture>(4);
    objects.add(std::make_shared<sphere>(
        point3{0, -1000, 0}, 1000, std::make_shared<lambertian>(pertext)));
    objects.add(std::make_shared<sphere>(
        point3{0, 2, 0}, 2, std::make_shared<lambertian>(pertext)));

    return objects;
}

int main() {
    for (int i = 0; i < 22; i++)
        random_double();

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // Camera & World
    point3 lookfrom{13, 2, 3};
    point3 lookat{0, 0, 0};
    vec3 up{0, 1, 0};
    double focus_distance = 10.0;
    double aperture = 0.1;
    double vfov = 20;

    hittable_list world;

    switch (3) {
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