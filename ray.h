#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray{
public:
    ray(){}//default constructor
    ray(const point3& origin, const vec3& direction) : m_origin(origin), m_dir(direction){};

    const point3& origin() const {return m_origin;}//getter for immutable reference to origin member
    const vec3& direction() const {return m_dir;}//getter for direction

    point3 at(double t) const {//returns the point on the line of the vector i.e. t could be time
        return m_origin + t*m_dir;
    }
private:
    point3 m_origin;
    vec3 m_dir;
};

#endif