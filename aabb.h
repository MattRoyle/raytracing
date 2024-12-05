#ifndef AABB_H
#define AABB_H

#include "headers.h"

class aabb {//axis aligned bounding box
  public:
    interval x, y, z;//bounding intervals in each axis
    static const aabb empty, universe;

    aabb() {} // default AABB is empty, since intervals are empty by default.

    aabb(const interval& x, const interval& y, const interval& z)
      : x(x), y(y), z(z) {
        pad_to_min();
      }

    aabb(const point3& a, const point3& b) {
        // a and b as extrema for the bounding box, particular minimum/maximum for every axis
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);

        pad_to_min();
    }

    aabb(const aabb& box0, const aabb& box1) {//creates a box connecting the two inputs
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
        pad_to_min();
    }

    const interval& axis_interval(int n) const {//1 -> y, 2 -> z, else -> x
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_orig = r.origin();
        const vec3&   ray_dir  = r.direction();

        for (int axis = 0; axis < 3; axis++) {
            const interval& ax = axis_interval(axis);//interval for the axis
            const double adinv = 1.0 / ray_dir[axis];//inverse direction of ray for axis

            auto t0 = (ax.min - ray_orig[axis]) * adinv;//t at the min of the slab
            auto t1 = (ax.max - ray_orig[axis]) * adinv;//t at the max

            if (t0 < t1) {//t0 must be entry and t1 exit points, is false when there is a NaN from adinv
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)//if any point in the max is <= ray min then some doesn't overlap
                return false;//so the ray doesn't intersect on all axies
        }
        return true;
    }
    int longest_axis() const {
        // Returns the index of the longest axis of the bounding box.

        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }
    

private:
    void pad_to_min() {//increases bounds to a min if necessary
        double min = 0.0001;
        if (x.size() < min) x = x.expand(min);
        if (y.size() < min) y = y.expand(min);
        if (z.size() < min) z = z.expand(min);
    }
};

const aabb aabb::empty    = aabb(interval::empty,    interval::empty,    interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

#endif