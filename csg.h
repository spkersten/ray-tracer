#pragma once

#include "./hittable.h"

using interval = std::pair<hit_record, hit_record>;

inline bool get_interval(
    const hittable& h, const ray& r, double t_min, double t_max, interval& interval
);

class difference : public hittable {
public:
    // a - b
    difference(
        std::shared_ptr<hittable> a_, std::shared_ptr<hittable> b_
    ) : a(a_), b(b_) {}

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
};

class intersection : public hittable {
public:
    // a & b
    intersection(
        std::shared_ptr<hittable> a_, std::shared_ptr<hittable> b_
    ) : a(a_), b(b_) {
        aabb box_a;
        has_box = true;
        if (!a->bounding_box(0, 1, box_a)) {
            has_box = false;
        }
        aabb box_b;
        if (!b->bounding_box(0, 1, box_b)) {
            has_box = false;
        }
        if (has_box) {
            box = enclosed_box(box_a, box_b);
        }
    }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        interval interval_a;
        interval_a.second.t = -infinity;
        interval interval_b;
        interval_b.second.t = -infinity;

        bool keep_a = false;
        bool keep_b = false;

        do {
            if (!keep_a && !get_interval(*a, r, t_min, t_max, interval_a)) {
                return false;
            }
            if (!keep_b && !get_interval(*b, r, t_min, t_max, interval_b)) {
                return false;
            }

            auto& first = interval_a.first.t > interval_b.first.t ? interval_a.first : interval_b.first;
            auto& second = interval_a.second.t < interval_b.second.t ? interval_a.second : interval_b.second;
            if (first.t < second.t) {
                if (t_min < first.t && first.t < t_max) {
                    rec = first;
                    return true;
                }
                if (t_min < second.t && second.t < t_max) {
                    rec = second;
                    return true;
                }
            }

            keep_a = interval_a.second.t > interval_b.second.t;
            keep_b = interval_b.second.t > interval_a.second.t;
        } while (true);

        return false;
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = box;
        return has_box;
    }

private:
    const std::shared_ptr<hittable> a;
    const std::shared_ptr<hittable> b;
    aabb box;
    bool has_box;
};

class fusion : public hittable {
public:
    // a + b
    fusion(
        std::shared_ptr<hittable> a_, std::shared_ptr<hittable> b_
    ) : a(a_), b(b_) {
        aabb box_a;
        has_box = true;
        if (!a->bounding_box(0, 1, box_a)) {
            has_box = false;
        }
        aabb box_b;
        if (!b->bounding_box(0, 1, box_b)) {
            has_box = false;
        }
        if (has_box) {
            box = surrounding_box(box_a, box_b);
        }
    }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        hit_record rec_a;
        hit_record rec_b;
        
        bool hit_a = a->hit(r, t_min, infinity, rec_a);
        bool hit_b = b->hit(r, t_min, infinity, rec_b);

        while (hit_a || hit_b) {
            if (!hit_b) {
                rec = rec_a;
                return true;
            } else if (!hit_a) {
                rec = rec_b;
                return rec.t < t_max;
            } else { // both hit
                if (rec_a.front_face && rec_b.front_face) {
                    rec = rec_a.t < rec_b.t ? rec_a : rec_b;
                    return rec.t < t_max;
                } else if (!rec_a.front_face && !rec_b.front_face) {
                    rec = rec_a.t > rec_b.t ? rec_a : rec_b;
                    return rec.t < t_max;
                } else {
                    // inside
                    if (rec_a.front_face) { 
                        if (rec_a.t < rec_b.t) {
                            if (rec_b.t > t_max) {
                                return false;
                            } else {
                                // hit a again to find end
                                hit_a = a->hit(r, rec_a.t + 0.0001, infinity, rec_a);
                            }
                        } else {
                            rec = rec_b;
                            return rec.t < t_max;
                        }
                    } else { // rec_b.front_face
                        if (rec_b.t < rec_a.t) {
                            if (rec_a.t > t_max) {
                                return false;
                            } else {
                                // hit b again to find end  
                                hit_b = b->hit(r, rec_b.t + 0.0001, infinity, rec_b);
                            }
                        } else {
                            rec = rec_a;
                            return rec.t < t_max;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = box;
        return has_box;
    }

private:
    const std::shared_ptr<hittable> a;
    const std::shared_ptr<hittable> b;
    aabb box;
    bool has_box;
};

inline bool get_interval(
    const hittable& h, const ray& r, double t_min, double t_max, interval& interval
) {
    hit_record& rec_a0 = interval.first;
    hit_record& rec_a1 = interval.second;

    do {
        if (!h.hit(r, rec_a1.t + 0.0001, infinity, rec_a0)) {
            return false;
        }
        if (!h.hit(r, rec_a0.t + 0.0001, infinity, rec_a1)) {
            return false;
        }
        if (rec_a0.t > t_max) {
            return false;
        }
    } while (rec_a1.t < t_min);

    return true;
}
