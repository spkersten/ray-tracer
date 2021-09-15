#pragma once

#include "./vec3.h"

class texture {
public:
    virtual color value(double u, double v, const point3& p) const = 0;

    virtual ~texture() {}
};

class solid_color : public texture {
public:
    solid_color(color c) : _color(c) {}

    solid_color(double red, double green, double blue)
      : solid_color(color{red, green, blue}) {}

    color value(double, double, const point3&) const override {
        return _color;
    }

private:
    color _color;
};
