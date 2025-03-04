#ifndef MATERIAL_H
#define MATERIAL_H

#include "headers.h"

#include "texture.h"

class hit_record;

class material {
  public:
    virtual ~material() = default;

	// get colour emitted using UV of texture
	virtual colour emitted(double u, double v, const point3& pos) const {
        return colour(0,0,0);
    }

    virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         colour& attenuation,
                         ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
  public:
    lambertian(const colour& albedo) : m_texture(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : m_texture(tex) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;//prevents case where the random is close to the inverse of the normal 

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = m_texture->value(rec.u, rec.v, rec.p);
        return true;
    }

  private:
    shared_ptr<texture> m_texture;
};
class metal : public material {
  public:
    metal(const colour& albedo, double fuzz) : m_albedo(albedo), m_fuzz(fuzz) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);//perfect reflection
        reflected = unit_vector(reflected) + (m_fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected, r_in.time());
        attenuation = m_albedo;
         return (dot(scattered.direction(), rec.normal) > 0);//only true if the reflected ray is same hemisphere as the normal
         //fuzz may cause the ray to go back into the object
    }

  private:
    colour m_albedo;
    double m_fuzz;//fuzz varies the reflected ray angle randomly, creating less sharp reflections
};

class dielectric : public material {
  public:
    dielectric(double refraction_index) : m_refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered)
    const override {
        attenuation = colour(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/m_refraction_index) : m_refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

  private:
    double m_refraction_index;// ratio of the material's refractive index over the refractive index of the enclosing media

    static double reflectance(double cosine, double refraction_index) {//Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};

class diffuse_light : public material {
  public:
    // constructor to emit light using a texture
    diffuse_light(shared_ptr<texture> tex) : texture(tex){}

    // constructor to emit light with a single colour
    diffuse_light(const colour& emit_color) : texture(make_shared<solid_color>(emit_color)) {}

    colour emitted(double u, double v, const point3& pos) const override {
        return texture->value(u,v,pos);
    }

  private:
    shared_ptr<texture> texture;
};

class isotropic : public material {
  public:
	isotropic(const colour& albedo) : tex(make_shared<solid_color>(albedo)) {}
	isotropic(shared_ptr<texture> tex) : tex(tex) {}
  
	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		scattered = ray(rec.p, random_unit_vector(), r_in.time());
		attenuation = tex->value(rec.u, rec.v, rec.p);
		return true;
	}
  
  private:
	shared_ptr<texture> tex;
};
  

#endif