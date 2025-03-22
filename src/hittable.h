#ifndef HITTABLE_H
#define HITTABLE_H

#include "headers.h"

#include "aabb.h"

class material;

class hit_record {//data for a hit, to simplify function parameters
  public:
    point3 p;//coordinate of the ray intersection/hit
    vec3 normal;//normal of the object at the ray hit point
    double t;// hit point along the ray
    bool front_face;
    shared_ptr<material> mat;//material of hit object
    double u,v;//surface coords of the hit
    void set_face_normal(const ray& r, const vec3& outward_normal) {//outward_normal is assumed to have unit length.
        front_face = dot(r.direction(), outward_normal) < 0;//if dot product of ray and outward normal is positive then its inside the sphere
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
  public:
    virtual ~hittable() = default;

    // virtal function that tests if the ray hits objects
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;// ray interval restricts t to between a min and max
    //get the bounding box for this object
    virtual aabb bounding_box() const = 0;

	//get the pdf value from a given direction
	virtual double pdf_value(const point3& origin, const vec3& direction) const {
        return 0.0;
    }

	//get a direction vector from a random point on the surface from the origin
    virtual vec3 random(const point3& origin) const {
        return vec3(1,0,0);
    }
};

class translate : public hittable{
  public:

  	translate(shared_ptr<hittable> object, const vec3& offset)
	: object(object), offset(offset)
	{
		bbox = object->bounding_box() + offset;
	}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      	// Move the ray backwards by the offset
      	ray offset_r(r.origin() - offset, r.direction(), r.time());

      	// Determine whether an intersection exists along the offset ray (and if so, where)
      	if (!object->hit(offset_r, ray_t, rec))
          	return false;

      	// Move the intersection point forwards by the offset
      	rec.p += offset;

      	return true;
    }

	aabb bounding_box() const override { return bbox; }

  private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};

class rotate_y : public hittable {
  public:
  	rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
		auto radians = degrees_to_radians(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		bbox = object->bounding_box();

		point3 min( INF,  INF,  INF);
		point3 max(-INF, -INF, -INF);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i*bbox.x.max + (1-i)*bbox.x.min;
					auto y = j*bbox.y.max + (1-j)*bbox.y.min;
					auto z = k*bbox.z.max + (1-k)*bbox.z.min;

					auto newx =  cos_theta*x + sin_theta*z;
					auto newz = -sin_theta*x + cos_theta*z;

					vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						min[c] = std::fmin(min[c], tester[c]);
						max[c] = std::fmax(max[c], tester[c]);
					}
				}
			}
		}
		bbox = aabb(min, max);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

		// Transform the ray from world space to object space.

		auto origin = point3(
			(cos_theta * r.origin().x()) - (sin_theta * r.origin().z()),
			r.origin().y(),
			(sin_theta * r.origin().x()) + (cos_theta * r.origin().z())
		);

		auto direction = vec3(
			(cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
			r.direction().y(),
			(sin_theta * r.direction().x()) + (cos_theta * r.direction().z())
		);

		ray rotated_r(origin, direction, r.time());

		// Determine whether an intersection exists in object space (and if so, where).

		if (!object->hit(rotated_r, ray_t, rec))
			return false;

		// Transform the intersection from object space back to world space.

		rec.p = point3(
			(cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
			rec.p.y(),
			(-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
		);

		rec.normal = vec3(
			(cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
			rec.normal.y(),
			(-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
		);

		return true;
	}
	aabb bounding_box() const override { return bbox; }

  private:
	shared_ptr<hittable> object;
	double sin_theta;
	double cos_theta;
	aabb bbox;
};
#endif