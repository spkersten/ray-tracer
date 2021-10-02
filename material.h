#pragma once

#include "./ray.h"
#include "./color.h"
#include "./pdf.h"

struct hit_record;

class scatter_record {
public:
    color attenuation;
    std::shared_ptr<pdf> pdf;
    ray skip_pdf_ray;
};

class material {
public:
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const {
        return false;
    }

    virtual double scattering_pdf(
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const {
        return 0;
    }

    virtual color emitted(const ray& r_in, const hit_record& rec) const {
        return color{0, 0, 0};
    }

    virtual ~material() {}
};
