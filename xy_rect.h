#pragma once

#include "./hittable.h"
#include "./vec3.h"

class xy_rect : public hittable {
public:
    // _x0 < _x1 and _y0 < _y1
    xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, std::shared_ptr<material> _material)
      : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), material(_material) {}
    
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        auto t = (k - r.origin().z()) / r.direction().z();
        if (t < t_min || t > t_max) {
            return false;
        }
        auto x = r.at(t).x();
        auto y = r.at(t).y();
        if (x < x0 || x > x1 || y < y0 || y > y1) {
            return false;
        }
        rec.t = t;
        rec.set_face_normal(r, vec3{0, 0, 1});
        rec.material = material;
        rec.p = r.at(t);
        rec.u = (x - x0) / (x1 - x0);
        rec.v = (y - y0) / (y1 - y0);
        return true;
    }

    bool bounding_box(double time0, double time, aabb& output_box) const override {
        // Bounding box must have non-zero size in all dimensions
        output_box = aabb{point3{x0, y0, k - 0.0001}, point3{x1, y1, k + 0.0001}};
        return true;
    }

private:
    double x0;
    double x1;
    double y0;
    double y1;
    double k;
    std::shared_ptr<material> material;
};

class xz_rect : public hittable {
public:
    // _x0 < _x1 and _z0 < _z1
    xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, std::shared_ptr<material> _material)
      : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), material(_material) {}
    
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        auto t = (k - r.origin().y()) / r.direction().y();
        if (t < t_min || t > t_max) {
            return false;
        }
        auto x = r.at(t).x();
        auto z = r.at(t).z();
        if (x < x0 || x > x1 || z < z0 || z > z1) {
            return false;
        }
        rec.t = t;
        rec.set_face_normal(r, vec3{0, 1, 0});
        rec.material = material;
        rec.p = r.at(t);
        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        return true;
    }

    bool bounding_box(double time0, double time, aabb& output_box) const override {
        // Bounding box must have non-zero size in all dimensions
        output_box = aabb{point3{x0, k - 0.0001, z0}, point3{x1, k + 0.0001, z1}};
        return true;
    }

private:
    double x0;
    double x1;
    double z0;
    double z1;
    double k;
    std::shared_ptr<material> material;
};


class yz_rect : public hittable {
public:
    // _y0 < _y1 and _z0 < _z1
    yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, std::shared_ptr<material> _material)
      : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), material(_material) {}
    
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        auto t = (k - r.origin().x()) / r.direction().x();
        if (t < t_min || t > t_max) {
            return false;
        }
        auto y = r.at(t).y();
        auto z = r.at(t).z();
        if (y < y0 || y > y1 || z < z0 || z > z1) {
            return false;
        }
        rec.t = t;
        rec.set_face_normal(r, vec3{1, 0, 0});
        rec.material = material;
        rec.p = r.at(t);
        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);
        return true;
    }

    bool bounding_box(double time0, double time, aabb& output_box) const override {
        // Bounding box must have non-zero size in all dimensions
        output_box = aabb{point3{k - 0.0001, y0, z0}, point3{k + 0.0001, y1, z1}};
        return true;
    }

private:
    double y0;
    double y1;
    double z0;
    double z1;
    double k;
    std::shared_ptr<material> material;
};