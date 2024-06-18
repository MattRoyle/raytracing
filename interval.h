#ifndef INTERVAL_H
#define INTERVAL_H
#include "headers.h"
class interval {
  public:
    double min, max;

    interval() : min(+INF), max(-INF) {} // Default interval is empty

    interval(double min, double max) : min(min), max(max) {}

    interval(const interval& a, const interval& b) {// construct interval tightly enclosing two intervals
        min = a.min <= b.min ? a.min : b.min;//min is minimum of the intervals
        max = a.max >= b.max ? a.max : b.max;//max is maximum of the intervals
    }

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }
    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    interval expand(double delta) const {//pads the interval
        auto padding = delta/2;
        return interval(min - padding, max + padding);
    }

    static const interval empty, universe;
};

const interval interval::empty    = interval(+INF, -INF);
const interval interval::universe = interval(-INF, +INF);

#endif