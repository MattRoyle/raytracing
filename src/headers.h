#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>


// C++ Std Usings
using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;
inline float Q_rsqrt(float number){//quake inverse sqrt algorithm
    long i;
    float x2, y;
    const float threehalfs = 1.5f;
    x2 = number * 0.5f;
    y= number;
    i = * (long *) &y; // store the bits of the float y in a long by casting the memory address to y from float to long then dereferencing
    i = 0x5f3759df - (i >> 1); // bit shifts the "float" to the right, including the exponent which becomes 1/2 then negate for -1/2
    // the hex number is from simplifying and is const = 3/2 * 2^23(127 - u),
    // where u is a error offset for log(1+x) =~ x + u, u= 0.0438 and the rest is from the IEEE 754 bit definition for float
    y = * (float *) &i;//store the long as a float
    y = y * ( threehalfs - (x2 * y * y));// y = 1/y^2 - x, newtons approximation, improves presision of functions, could be repeated

    return y;
}
// Constants

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

template <typename T>
inline T lerp(T start, T end, double at){
    return (1.0-at)*start + at*end;
}

inline double random_double() {// random 0 to 1
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {// random min to max
    return min + (max-min)*random_double();
}

inline int random_int(int min, int max) {// random min to max
    return int(random_double(min, max+1));
}


// Common Headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif