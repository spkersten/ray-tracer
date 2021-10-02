#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <chrono>
#include <iomanip>

#include "./bvh.h"
#include "./camera.h"
#include "./color.h"
#include "./thread_pool.h"
#include "./material.h"

class camera_config {
public:
    point3 lookfrom{13, 2, 3};
    point3 lookat{0, 0, 0};
    vec3 up{0, 1, 0};
    double focus_distance = 10.0;
    double aperture = 0.0;
    double vfov = 20;
};

class scene {
public:
    void render() {
        auto image_height = static_cast<int>(image_width / aspect_ratio);

        camera camera{
            cam.lookfrom,
            cam.lookat,
            cam.up,
            cam.vfov,
            aspect_ratio,
            cam.aperture,
            cam.focus_distance
        };

        bvh_node world_tree{
            world,
            0,
            0,
        };

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
                        pixel_color += ray_color(r, world_tree, max_depth);
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

        p.run(nthreads);

        // Write the image

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        
        for (int j = image_height - 1; j >= 0; --j) {
            for (int i = 0; i < image_width; ++i) {            
                write_color(std::cout, pixel_colors[j * image_width + i], samples_per_pixel);
            }
        }

        std::cerr << "\nDone\n";
    }

public:
    hittable_list world;
    camera_config cam;
    std::optional<color> background = std::nullopt;

    int image_width = 100;
    double aspect_ratio = 1.0;
    int samples_per_pixel = 10;
    int max_depth = 50;
    int nthreads = 4;

private:
    color ray_color(const ray& r, const hittable& world_tree, int depth) {
        if (depth <= 0) {
            return color{0, 0, 0};
        }

        hit_record rec;
        if (world.hit(r, 0.001, infinity, rec)) {
            // Normals: 
            //return 0.5 * (rec.normal + color{1, 1, 1});

            ray scattered;
            color attenuation;
            color emitted = rec.material->emitted(rec.u, rec.v, rec.p);

            if (rec.material->scatter(r, rec, attenuation, scattered)) {
                return emitted + attenuation * ray_color(scattered, world_tree, depth - 1);
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
};
