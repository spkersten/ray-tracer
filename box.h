#pragma once

#include "./hittable.h"
#include "./hittable_list.h"
#include "./xy_rect.h"

class box : public hittable {
public:
    box(const point3& p0, const point3& p1, std::shared_ptr<material> material)
      : _min(p0), _max(p1) 
    {
        sides.add(std::make_shared<xy_rect>(
            _min.x(), _max.x(), _min.y(), _max.y(), _min.z(), -1, material));
        sides.add(std::make_shared<xy_rect>(
            _min.x(), _max.x(), _min.y(), _max.y(), _max.z(), 1, material));

        sides.add(std::make_shared<xz_rect>(
            _min.x(), _max.x(), _min.z(), _max.z(), _min.y(), -1, material));
        sides.add(std::make_shared<xz_rect>(
            _min.x(), _max.x(), _min.z(), _max.z(), _max.y(), 1, material));

        sides.add(std::make_shared<yz_rect>(
            _min.y(), _max.y(), _min.z(), _max.z(), _min.x(), -1, material));
        sides.add(std::make_shared<yz_rect>(
            _min.y(), _max.y(), _min.z(), _max.z(), _max.x(), 1, material));
    }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        return sides.hit(r, t_min, t_max, rec);
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = aabb{_min, _max};
        return true;
    }

private:
    point3 _min;
    point3 _max;
    hittable_list sides;
};
