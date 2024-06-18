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