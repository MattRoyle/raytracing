#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
  public:
    sphere(const point3& center, double radius, shared_ptr<material> mat) : m_center(center), m_radius(fmax(0,radius)), m_mat(mat) {
      
    }

    bool hit(const ray& r, interval ray_t, hit_record& record) const override {
        vec3 offsetCenter= m_center - r.origin();// sphere's position relative to the ray start
        auto a = r.direction().length_squared(); //derived value for a in quadratic to find the t intercections with the sphere
        auto h = dot(r.direction(), offsetCenter);
        auto c = offsetCenter.length_squared() - m_radius*m_radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto discriminant_sqrt = sqrt(discriminant);

        // check minus then plus of quadratic
        auto root = (h - discriminant_sqrt) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + discriminant_sqrt) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        //records the hit
        record.t = root;
        record.p = r.at(record.t);
        vec3 outward_normal = (record.p - m_center) / m_radius;
        record.set_face_normal(r, outward_normal);
        record.normal = (record.p - m_center) / m_radius;
        record.mat = m_mat;
        return true;
    }

  private:
    point3 m_center;
    double m_radius;
    shared_ptr<material> m_mat;
};

#endif