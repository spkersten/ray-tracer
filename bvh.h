#pragma once

#include <algorithm>
#include <memory>

#include "./hittable_list.h"
#include "./utils.h"


bool box_x_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b);

bool box_y_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b);

bool box_z_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b);

// Bounding volume hierarchy
class bvh_node : public hittable {
public:
    bvh_node(const hittable_list& list, double time0, double time1)
        : bvh_node(list.objects, 0, list.objects.size(), time0, time1) {}

    bvh_node(
        const std::vector<std::shared_ptr<hittable>>& src_objects, 
        size_t start, 
        size_t end, 
        double time0, 
        double time1
    ) {
        if (src_objects.empty()) {
            std::cerr << "No source objects in bvh_node constructor.\n";
            exit(1);
        }

        auto objects = src_objects;

        int axis = random_int(0, 2);
        auto comparator = axis == 0 ? box_x_compare
                        : axis == 1 ? box_y_compare
                        : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            if (comparator(objects[start], objects[start + 1])) {
                left = objects[start];
                right = objects[start + 1];
            } else {
                left = objects[start + 1];
                right = objects[start];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + object_span / 2;
            left = std::make_shared<bvh_node>(objects, start, mid, time0 , time1);
            right = std::make_shared<bvh_node>(objects, mid, end, time0 , time1);
        }

        aabb left_box, right_box;

        if (!left->bounding_box(time0, time1, left_box)
          || !right->bounding_box(time0, time1, right_box)
        ) {
            std::cerr << "No bounding box in bvh_node constructor.\n";
            exit(1);
        }

        box = surrounding_box(left_box, right_box);
    }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        if (!box.hit(r, t_min, t_max)) {
            return false;
        }

        bool hit_left = left->hit(r, t_min, t_max, rec);
        bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

        return hit_left || hit_right;
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = box;
        return true;
    }

    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    aabb box;
};

bool box_x_compare(
    const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b
) {
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b)) {
        std::cerr << "No bounding box in bvh_node constructor.\n";
        exit(1);
    }

    return box_a.min().x() < box_b.min().x();
}

bool box_y_compare(
    const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b
) {
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b)) {
        std::cerr << "No bounding box in bvh_node constructor.\n";
        exit(1);
    }

    return box_a.min().y() < box_b.min().y();
}

bool box_z_compare(
    const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b
) {
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) {
        std::cerr << "No bounding box in bvh_node constructor.\n";
        exit(1);
    }

    return box_a.min().z() < box_b.min().z();
}
