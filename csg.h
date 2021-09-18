#pragma once

#include "./hittable.h"

using interval = std::pair<hit_record, hit_record>;

class difference : public hittable {
public:
    // a - b
    difference(
        std::shared_ptr<hittable> a_, std::shared_ptr<hittable> b_, 
        std::shared_ptr<material> material_ = nullptr
    ) : a(a_), b(b_), material(material_) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        auto a_intervals = get_intervals(r, *a);
        auto b_intervals = get_intervals(r, *b);
        auto intervals = set_difference(a_intervals, b_intervals);
        for (const interval& interval : intervals) {
            if (t_min < interval.first.t && interval.first.t < t_max) {
                rec = interval.first;
                return true;
            }
            if (t_min < interval.second.t && interval.second.t < t_max) {
                rec = interval.second;
                return true;
            }
        }
        return false;
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        return a->bounding_box(time0, time1, output_box);
    }

private:
    static std::vector<interval> get_intervals(const ray& r, const hittable& obj) {
        hit_record rec;
        hit_record rec_previous;
        rec_previous.t = -infinity;
        rec_previous.front_face = false;
        std::vector<interval> intervals;

        while (obj.hit(r, rec_previous.t + 0.0001, infinity, rec)) {
            if (rec_previous.front_face == rec.front_face) {
                std::cerr << "Expected front face in difference: " << rec.front_face << "\n";
                exit(1);
            }
            if (!rec.front_face) {
                intervals.emplace_back(rec_previous, rec);
            }
            rec_previous = rec;
        }
        return intervals;
    }

    static std::vector<interval> set_difference(std::vector<interval> a, std::vector<interval> b) {
        std::vector<interval> result;

        int j = 0;
        for (int i = 0; i < a.size(); i++) {
            hit_record start = a[i].first;
            hit_record end = a[i].second;

            while (j < b.size()) {
                if (b[j].second.t < start.t) {
                    j++;
                    continue;
                }
                if (b[j].first.t > end.t) {
                    break;
                }

                if (b[j].first.t < start.t) {
                    start = b[j].second;
                    start.front_face = !start.front_face;
                } else {
                    auto local_end = b[j].first;
                    local_end.front_face = !local_end.front_face;
                    result.emplace_back(start, local_end);
                    start = b[j].second;
                    start.front_face = !start.front_face;
                }
                j++;
            }

            if (end.t > start.t) {
                result.emplace_back(start, end);
            }
        }

        return result;
    }

public:
    const std::shared_ptr<hittable> a;
    const std::shared_ptr<hittable> b;
    const std::shared_ptr<material> material;
};
