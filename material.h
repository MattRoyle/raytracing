#ifndef MATERIAL_H
#define MATERIAL_H

#include "headers.h"

class hit_record;

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         color& attenuation,
                         ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
  public:
    lambertian(const color& albedo) : m_albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;//prevents case where the random is close to the inverse of the normal 

        scattered = ray(rec.p, scatter_direction);
        attenuation = m_albedo;
        return true;
    }

  private:
    color m_albedo;// albedo aka fractional reflectance, i.e. amount reflected
};
class metal : public material {
  public:
    metal(const color& albedo, double fuzz) : m_albedo(albedo), m_fuzz(fuzz) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);//perfect reflection
        reflected = unit_vector(reflected) + (m_fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = m_albedo;
         return (dot(scattered.direction(), rec.normal) > 0);//only true if the reflected ray is same hemisphere as the normal
         //fuzz may cause the ray to go back into the object
    }

  private:
    color m_albedo;
    double m_fuzz;//fuzz varies the reflected ray angle randomly, creating less sharp reflections
};
#endif