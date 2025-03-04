#ifndef INTERVAL_H
#define INTERVAL_H
#include "headers.h"
class interval {
  public:
    double min, max;

    // construct default interval (empty), spanning from +inf to -inf
    interval() : min(+INF), max(-INF) {} 

    // construct interval from min to max
    interval(double min, double max) : min(min), max(max) {}

    // construct interval by combining two intervals
    interval(const interval& a, const interval& b) {// construct interval tightly enclosing two intervals
        min = a.min <= b.min ? a.min : b.min;//min is minimum of the intervals
        max = a.max >= b.max ? a.max : b.max;//max is maximum of the intervals
    }

    // size/length of an interval
    double size() const {
        return max - min;
    }

    // if a value is strickly inside an interval
    bool contains(double x) const {
        return min <= x && x <= max;
    }

    // if a value is inside an interval
    bool surrounds(double x) const {
        return min < x && x < max;
    }
    
    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    //pads the interval
    interval expand(double delta) const {
        auto padding = delta/2;
        return interval(min - padding, max + padding);
    }

    static const interval empty, universe;
};

const interval interval::empty    = interval(+INF, -INF);
const interval interval::universe = interval(-INF, +INF);

interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif