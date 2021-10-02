#pragma once

#include "./vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : _origin(origin), _direction(direction)
    {}

    point3 origin() const { 
        return _origin;
    }

    // Not normalized!
    vec3 direction() const {
        return _direction;
    }

    point3 at(double t) const {
        return _origin + t * _direction;
    }

private:
    point3 _origin;
    vec3 _direction;
};
