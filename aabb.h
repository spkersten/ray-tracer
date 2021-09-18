#pragma once

#include "./vec3.h"
#include "./ray.h"

// Axis-aligned bounding box
class aabb {
public:
    aabb() {}

    // All coefficients of a must be smaller than the corresponding 
    // coefficients of b.
    // Bounding box must have non-zero size in all dimensions.
    aabb(const point3& a, const point3& b) : _min(a), _max(b) {}

    point3 min() const {
        return _min;
    }

    point3 max() const {
        return _max;
    }

    bool hit(const ray& r, double t_min, double t_max) const {
        for (int i = 0; i < 3; i++) {
            auto invD = 1.0 / r.direction()[i];
            auto t0 = (min()[i] - r.origin()[i]) * invD;
            auto t1 = (max()[i] - r.origin()[i]) * invD;
            if (invD < 0.0) {
                std::swap(t0, t1);
            }
            t_min = std::max(t_min, t0);
            t_max = std::min(t_max, t1);
            if (t_max <= t_min) {
                return false;
            }
        }
        return true;
    }

private:
    point3 _min;
    point3 _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
    point3 small{
        std::min(box0.min().x(), box1.min().x()),
        std::min(box0.min().y(), box1.min().y()),
        std::min(box0.min().z(), box1.min().z())
    };
    point3 large{
        std::max(box0.max().x(), box1.max().x()),
        std::max(box0.max().y(), box1.max().y()),
        std::max(box0.max().z(), box1.max().z())
    };
    return aabb{small, large};
}

aabb enclosed_box(aabb box0, aabb box1) {
    point3 small{
        std::max(box0.min().x(), box1.min().x()),
        std::max(box0.min().y(), box1.min().y()),
        std::max(box0.min().z(), box1.min().z())
    };
    point3 large{
        std::min(box0.max().x(), box1.max().x()),
        std::min(box0.max().y(), box1.max().y()),
        std::min(box0.max().z(), box1.max().z())
    };
    return aabb{small, large};
}
