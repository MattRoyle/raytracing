#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"

#include "perlin.h"
#include "image_manager.h"

class texture {
  public:
    virtual ~texture() = default;

    virtual colour value(double u, double v, const point3& pos) const = 0;//texture coords uv, and position p
};

class solid_color : public texture {
  public:
    solid_color(const colour& albedo) : m_albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(colour(red,green,blue)) {}

    colour value(double u, double v, const point3& p) const override {//constant colour, ignores surface and position
        return m_albedo;
    }

  private:
    colour m_albedo;
};

class checker_texture : public texture {
  public:
    checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(double scale, const colour& c1, const colour& c2)
      : inv_scale(1.0 / scale),
        even(make_shared<solid_color>(c1)),
        odd(make_shared<solid_color>(c2))
    {}

    colour value(double u, double v, const point3& p) const override {
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

class image_texture : public texture {
  public:
    image_texture(const char* filename) : image(filename) {}

    colour value(double u, double v, const point3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return colour(0,1,1);

        // Clamp input texture coordinates to [0,1] x [1,0] fractional position
        u = interval(0,1).clamp(u);
        v = 1.0 - interval(0,1).clamp(v);  // Flip V to image coordinates

        auto i = int(u * image.width());//from u = i/(N_x - 1)
        auto j = int(v * image.height());// v = i/(N_y - 1)
        auto pixel = image.pixel_data(i,j);

        auto color_scale = 1.0 / 255.0;
        return colour(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

  private:
    rtw_image image;
};

class noise_texture : public texture {
  public:
    noise_texture() {}
    noise_texture(double scale) : m_scale(scale) {}
    colour value(double u, double v, const point3& p) const override {//grayscale gradient
        return colour(.5, .5, .5) * (1 + sin(m_scale * p.z() + 10 * noise.octave(p, 7)));//marbel effect wurg sin undulating the stripes
    }

  private:
    perlin noise;
    double m_scale;
};

#endif