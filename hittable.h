#ifndef HITTABLE_H
#define HITTABLE_H

#include "headers.h"

#include "aabb.h"

class material;

class hit_record {//data for a hit, to simplify function parameters
  public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    shared_ptr<material> mat;//material of hit object
    double u,v;//surface coords of the hit
    void set_face_normal(const ray& r, const vec3& outward_normal) {//outward_normal is assumed to have unit length.
        front_face = dot(r.direction(), outward_normal) < 0;//if dot product of ray and outward normal is positive then its inside the sphere
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {// abstract class for all "objects" that rays can hit
  public:
    virtual ~hittable() = default;

    //subtype polymorthism for abstract class hittable
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;// restricts t to between a min and max
    virtual aabb bounding_box() const = 0;//get the bounding box for this object
};

#endif