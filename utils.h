#pragma once

#include <random>

double clamp(double x, double min, double max) {
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    } else {
        return x;
    }
}

const double pi = 3.14159265358979;
const double infinity = std::numeric_limits<double>::infinity();

double random_double() {
    static auto distribution = std::uniform_real_distribution<double>(0.0, 1.0);
    static thread_local std::mt19937 generator;
    return distribution(generator);
}

// Returns a random real in [min, max)
double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max + 1));
}
