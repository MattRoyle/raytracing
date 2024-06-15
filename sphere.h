#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
  public:
    sphere(const point3& center, double radius) : center(center), radius(fmax(0,radius)) {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& record) const override {
        vec3 offsetCenter= center - r.origin();// sphere's position relative to the ray start
        auto a = r.direction().length_squared(); //derived value for a in quadratic to find the t intercections with the sphere
        auto h = dot(r.direction(), offsetCenter);
        auto c = offsetCenter.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto discriminant_sqrt = sqrt(discriminant);

        // check minus then plus of quadratic
        auto root = (h - discriminant_sqrt) / a;
        if (root <= ray_tmin || ray_tmax <= root) {
            root = (h + discriminant_sqrt) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        //records the hit
        record.t = root;
        record.p = r.at(record.t);
        vec3 outward_normal = (record.p - center) / radius;
        record.set_face_normal(r, outward_normal);
        record.normal = (record.p - center) / radius;

        return true;
    }

  private:
    point3 center;
    double radius;
};

#endif