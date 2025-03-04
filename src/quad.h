#ifndef QUAD_H
#define QUAD_H

#include "headers.h"

#include "hittable.h"

class quad : public hittable {
  public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
      : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);//gives the normal vector of the plane the u, v lies on
        normal = unit_vector(n);//store as unit vector
        D = dot(normal, Q);// D = n . Q

        w = n / dot(n,n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {//compute bounds for 4 verticies in quad
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);//uses 2 points constructor 
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);//combined diagonals contains the entire quad
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        
        auto denominator = dot(normal, r.direction());//demonimator of t = (D - n . P)/(n . d)
        //from n . v = D & R(t) = P + t * d

        // No hit if the ray is parallel to the plane, min value
        if (fabs(denominator) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        auto t = (D - dot(normal, r.origin())) / denominator;
        if (!ray_t.contains(t))
            return false;

        // Determine if the hitpoint is within the planar shape using plana coords
        auto intersection = r.at(t);//in world coords
        vec3 planar_hitpt_vector = intersection - Q;//truncate to plane cords
        //divide plane into regions alpha and beta s.t Q -> alpha_0, beta_0, v -> alpha_0, beta_1, u -> alpha_1, beta_0
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }
    virtual bool is_interior(double alpha, double beta, hit_record& rec) const {// given plane coords check if within the unit interval
        interval unit_interval = interval(0, 1);
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if (!unit_interval.contains(alpha) || !unit_interval.contains(beta))
            return false;

        rec.u = alpha;
        rec.v = beta;
        return true;
    }

  protected:
    point3 Q;//starting pos
    vec3 u, v;//u is width, v is height
    vec3 w; //constant for orienting the plane of the quad
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;//constant for the plane equation Ax + Bx + Cx = D
};


class triangle : public quad {
public:
    triangle(const point3& origin, const vec3& side_ab, const vec3& side_ac, shared_ptr<material> mat) 
    : quad(origin, side_ab, side_ac, mat){}

    void set_bounding_box() override{
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);//uses 2 points constructor 
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);//combined diagonals contains the entire quad
    }
    bool is_interior(double alpha, double beta, hit_record& rec) const override {
        if(!(alpha > 0 && beta > 0 && alpha + beta < 1)){return false;}

        rec.u = alpha;
        rec.v = beta;
        return true;
    }
};

class ellipse : public quad {
  public:
    ellipse(
        const point3& center, const vec3& side_ab, const vec3& side_ac, shared_ptr<material> mat
    ) : quad(center, side_ab, side_ac, mat)
    {}

    void set_bounding_box() override {
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);//uses 2 points constructor 
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);//combined diagonals contains the entire quad
    }

    bool is_interior(double a, double b, hit_record& rec) const override {
        if ((a*a + b*b) > 1)
            return false;

        rec.u = a/2 + 0.5;
        rec.v = b/2 + 0.5;
        return true;
    }
};


class annulus : public quad {
  public:
    annulus(
        const point3& center, const vec3& side_A, const vec3& side_B, double inner,
        shared_ptr<material> mat)
      : quad(center, side_A, side_B, mat), m_inner(inner)
    {}

    void set_bounding_box() override {
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);//uses 2 points constructor 
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);//combined diagonals contains the entire quad
    }

    bool is_interior(double a, double b, hit_record& rec) const override {
        auto center_dist = sqrt(a*a + b*b);
        if ((center_dist < m_inner) || (center_dist > 1))
            return false;

        rec.u = a/2 + 0.5;
        rec.v = b/2 + 0.5;
        return true;
    }

  private:
    double m_inner;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat)
{
    // Returns the six sides (box) that contains the two opposite vertices a & b
    auto sides = make_shared<hittable_list>();

    // Construct the two opposite vertices with the minimum and maximum coordinates
    auto min = point3(std::fmin(a.x(),b.x()), std::fmin(a.y(),b.y()), std::fmin(a.z(),b.z()));
    auto max = point3(std::fmax(a.x(),b.x()), std::fmax(a.y(),b.y()), std::fmax(a.z(),b.z()));

    // vectors to represent the distance from the min to the max on each axis
    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    //six cube faces/quads
    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front side
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right side
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}
#endif