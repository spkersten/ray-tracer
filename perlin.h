#pragma once

#include <array>

#include "./utils.h"
#include "./vec3.h"

class perlin {
public:
    perlin() {
        for (int i = 0; i < point_count; i++) {
            ranvec[i] = vec3::random(-1, 1).normalized();
        }
        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    double noise(const point3& p) const {
        auto u = p.x() - std::floor(p.x());
        auto v = p.y() - std::floor(p.y());
        auto w = p.z() - std::floor(p.z());
        auto i = static_cast<int>(std::floor(p.x()));
        auto j = static_cast<int>(std::floor(p.y()));
        auto k = static_cast<int>(std::floor(p.z()));
        vec3 c[2][2][2];

        for (int di = 0; di < 2; di++) {
            for (int dj = 0; dj < 2; dj++) {
                for (int dk = 0; dk < 2; dk++) {
                    c[di][dj][dk] = ranvec[
                        perm_x[(i + di) & 255] ^
                        perm_y[(j + dj) & 255] ^
                        perm_z[(k + dk) & 255]
                    ];
                }
            }
        }

        return perlin_interp(c, u, v, w);
    }

private:
    static const int point_count = 256;
    std::array<vec3, point_count> ranvec;
    std::array<int, point_count> perm_x;
    std::array<int, point_count> perm_y;
    std::array<int, point_count> perm_z;

    static std::array<int, point_count> perlin_generate_perm() {
        std::array<int, point_count> p;
        for (int i = 0; i < point_count; i++) {
            p[i] = i;
        }
        permute(p);
        return p;
    }

    static void permute(std::array<int, point_count>& p) {
        for (int i = point_count - 1; i > 0; i--) {
            std::swap(p[i], p[random_int(0, i)]);
        }
    }

    static double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    vec3 weight_v{u - i, v - j, w - k};
                    accum += (i * uu + (1 - i) * (1 - uu))
                           * (j * vv + (1 - j) * (1 - vv))
                           * (k * ww + (1 - k) * (1 - ww))
                           * dot(c[i][j][k], weight_v);
                }
            }
        }
        return accum;
    }
};
