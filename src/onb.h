#ifndef ONB_H
#define ONB_H
#include "vec3.h"
class onb
{
public:
    //sets up a orthonormal basis with the normal as the z
    onb(const vec3& n){
        axis[2] = unit_vector(n); //normalise the normal vector
        vec3 a = (fabs(axis[2].x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
        axis[1] = unit_vector(cross(axis[2], a)); // s vector
        axis[0] = cross(axis[2], axis[1]); //t vector can be found as perpendicular to the first two axes
    }
    const vec3& u() const { return axis[0]; }
    const vec3& v() const { return axis[1]; }
    const vec3& w() const { return axis[2]; }

    // Transform from basis coordinates to local space
    vec3 transform(const vec3& v) const {
        return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
    }
private:
    vec3 axis[3];
};
#endif