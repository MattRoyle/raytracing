#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"
class constant_medium : public hittable {
  public:
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex)
        : boundary(boundary), negative_inverse_density(-1/density),
        phase_function(make_shared<isotropic>(tex))
    {}
    constant_medium(shared_ptr<hittable> boundary, double density, const colour& albedo)
      : boundary(boundary), negative_inverse_density(-1/density),
        phase_function(make_shared<isotropic>(albedo))
    {}

    bool hit(const ray& r, interval ray_t, hit_record& record) const override {
        hit_record rec1, rec2;

        // Check if the ray hits the bounding box
        if (!boundary->hit(r, interval::universe, rec1))
            return false;

        // Check if the point hits the other side of the box (by adding small amount to the original hit t)
        if (!boundary->hit(r, interval(rec1.t+0.0001, INF), rec2))
            return false;

        // Clamp the hit t to the ray interval min & max
        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        // The ray must hit the first side first, not sure how this would be possible
        if (rec1.t >= rec2.t)
            return false;
        
        if (rec1.t < 0)
            rec1.t = 0;

        // Record the hit based of probability inside the volume, proportional to the density of the volume
        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = negative_inverse_density * std::log(random_double());//Use the density and a random value to get the hit distance

        if (hit_distance > distance_inside_boundary)
            return false;

        record.t = rec1.t + hit_distance / ray_length;//scale to the length of the ray then add it to the first intersection

        record.p = r.at(record.t);//get the point along the ray the hit occurred

        record.normal = vec3(1,0,0);  // arbitrary
        record.front_face = true;     // also arbitrary
        record.mat = phase_function;
        // assumes the volume boundary is convex (the ray will exit without rentering the boundary)
        return true;
    }

    aabb bounding_box() const override { return boundary->bounding_box(); }

  private:
    shared_ptr<hittable> boundary;
    double negative_inverse_density;
    shared_ptr<material> phase_function;
};
#endif