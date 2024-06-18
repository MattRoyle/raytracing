#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
  public:
    //stationary sphere
    sphere(const point3& center, double radius, shared_ptr<material> mat) : m_center(center), m_radius(fmax(0,radius)), m_mat(mat) {
        auto radius_vec = vec3(m_radius, m_radius, m_radius);
        bbox = aabb(m_center - radius_vec, m_center + radius_vec);
    }
     //moving Sphere
    sphere(const point3& center1, const point3& center2, double radius,
           shared_ptr<material> mat)
      : m_center(center1), m_radius(fmax(0,radius)), m_mat(mat), is_moving(true)
    {
        auto radius_vec = vec3(m_radius, m_radius, m_radius);
        aabb box1(center1 - radius_vec, center1 + radius_vec);
        aabb box2(center2 - radius_vec, center2 + radius_vec);
        bbox = aabb(box1, box2);

        displacement = center2 - center1;
    }
    bool hit(const ray& r, interval ray_t, hit_record& record) const override {
        point3 center = is_moving ? sphere_center(r.time()) : m_center;
        vec3 offsetCenter= center - r.origin();// sphere's position relative to the ray start
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
        vec3 outward_normal = (record.p - center) / m_radius;
        record.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, record.u, record.v);
        record.mat = m_mat;
        return true;
    }
    aabb bounding_box() const override { return bbox; }
  private:
    point3 m_center;
    double m_radius;
    shared_ptr<material> m_mat;
    bool is_moving;
    vec3 displacement;
    aabb bbox;

    point3 sphere_center(double time) const {
        // Linearly interpolate from center1 to center2 according to time, where t=0 yields
        // center1, and t=1 yields center2.
        return m_center + time*displacement;
    }

    static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + PI;//0 to PI

        u = phi / (2*PI);
        v = theta / PI;
    }
};

#endif