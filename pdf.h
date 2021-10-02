#pragma once

#include <memory>
#include <array>
#include <cmath>

#include "./onb.h"
#include "./vec3.h"
#include "./hittable_list.h"

class pdf {
public:
    virtual ~pdf() {}

    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

class sphere_pdf : public pdf {
public:
    double value(const vec3&) const override {
        return 1 / (4 * pi);
    }

    vec3 generate() const override {
        return random_unit_vector();
    }
};

// one hemisphere, cos(theta) weighted
class cosine_pdf : public pdf {
public:
    cosine_pdf(const vec3& w) {
        uvw.build_from_w(w);
    }

    double value(const vec3& direction) const override {
        const auto cos_theta = dot(direction.normalized(), uvw.w());
        return std::max(0.0, cos_theta) / pi;
    }

    vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

private:
    onb uvw;
};

// distribution of vectors form origin to a random point on a hittable
class hittable_pdf : public pdf {
public:
    hittable_pdf(const hittable& objects_, const point3& origin_)
      : objects(objects_), origin(origin_)
    {}

    double value(const vec3& direction) const override {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override {
        return objects.random(origin);
    }

private:
    const hittable& objects;
    point3 origin;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1, double p0_weight_ = 0.5)
      : p0_weight(p0_weight_), p{p0, p1}
    {}

    double value(const vec3& direction) const override {
        return p0_weight * p[0]->value(direction) + (1 - p0_weight) * p[1]->value(direction);
    }

    vec3 generate() const override {
        if (random_double() < p0_weight) {
            return p[0]->generate();
        } else {
            return p[1]->generate();
        }
    }

private:
    double p0_weight;
    std::array<std::shared_ptr<pdf>, 2> p;
};
