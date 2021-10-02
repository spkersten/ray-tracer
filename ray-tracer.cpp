#include <memory>

#include "./scene.h"

#include "./scenes/cornell_box.h"
#include "./scenes/cornell_box_2.h"
#include "./scenes/cornell_box_csg.h"
#include "./scenes/cornell_box_two_boxes.h"
#include "./scenes/cornell_smoke.h"
#include "./scenes/earth.h"
#include "./scenes/lens_setup.h"
#include "./scenes/random_balls.h"
#include "./scenes/simple_light.h"
#include "./scenes/three_spheres.h"
#include "./scenes/three_spheres_light.h"
#include "./scenes/two_perlin_spheres.h"

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
