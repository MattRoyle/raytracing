#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"

class texture {
  public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& pos) const = 0;//texture coords uv, and position p
};

class solid_color : public texture {
  public:
    solid_color(const color& albedo) : m_albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(color(red,green,blue)) {}

    color value(double u, double v, const point3& p) const override {//constant colour, ignores surface and position
        return m_albedo;
    }

  private:
    color m_albedo;
};

class checker_texture : public texture {
  public:
    checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(double scale, const color& c1, const color& c2)
      : inv_scale(1.0 / scale),
        even(make_shared<solid_color>(c1)),
        odd(make_shared<solid_color>(c2))
    {}

    color value(double u, double v, const point3& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;//resultant modulo 2 gives even or odd checkered pattern 

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);//returns the even or odd texture accordingly
    }

  private:
    double inv_scale;//input value scale?
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

#endif