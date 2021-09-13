#pragma once

#include <cmath>
#include <iostream>

#include "./utils.h"

using std::sqrt;

class vec3 {
public:
    vec3() : e{0, 0, 0} {}
    vec3(double x, double y, double z) : e{x, y, z} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-x(), -y(), -z()); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3 &v) {
        e[0] += v.x();
        e[1] += v.y();
        e[2] += v.z();
        return *this;
    }

    vec3& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const double t) {
        return *this *= 1/t;
    }

    double length() const {
        return sqrt(length_squared());
    }

    double length_squared() const {
        return x()*x() + y()*y() + z()*z();
    }

    vec3 normalized() const;

    static vec3 random() {
        return vec3{random_double(), random_double(), random_double()};
    }

    static vec3 random(double min, double max) {
        return vec3{random_double(min, max), random_double(min, max), random_double(min, max)};
    }

    bool near_zero() const {
        const auto s = 1e-8;
        return fabs(x()) < s && fabs(y()) < s && fabs(z()) < s;
    }

protected:
    double e[3];
};

using color = vec3;
using point3 = vec3;

vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

vec3 random_unit_vector() {
    return random_in_unit_sphere().normalized();
}

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.x(), t*v.y(), t*v.z());
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.x() * v.x()
         + u.y() * v.y()
         + u.z() * v.z();
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.y() * v.z() - u.z() * v.y(),
                u.z() * v.x() - u.x() * v.z(),
                u.x() * v.y() - u.y() * v.x());
}

inline vec3 vec3::normalized() const {
    return *this / length();
}

// Reflect v in n
vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3& v, const vec3& n, double refractive_index_ratio) {
    auto cos_theta = std::min(dot(-v, n), 1.0);
    vec3 r_out_perpendicular = refractive_index_ratio * (v + cos_theta * n);
    vec3 r_out_parallel = -sqrt(std::abs(1.0 - r_out_perpendicular.length_squared())) * n;
    return r_out_perpendicular + r_out_parallel;
}
