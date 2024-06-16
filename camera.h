#ifndef CAMERA_H
#define CAMERA_H

#include "headers.h"

#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 16.0 / 9.0;//aspect ration is ideal ratio
    int image_width = 400;
    int samples_per_pixel = 10;
    int    max_depth         = 10;   // Maximum number of ray bounces into scene
    void render(const hittable& world) {
        initialize();
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";//formatting the .ppm file
        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;//writes to the console
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rDone.                 \n";
    }

  private:
    int    image_height;
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 camera_center;
    point3 pixel_origin; // Location of pixel 0, 0
    vec3   pixel_delta_u; //horizontal pixel offset
    vec3   pixel_delta_v; // vertical pixel offset

    void initialize() {
       
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // calculate the image height, and ensure that it's at least 1.
        pixel_samples_scale = 1.0 / samples_per_pixel;
        auto focal_length = 1.0; //inital value of orthagonal dist from viewport
        camera_center = point3(0, 0, 0);
        // Viewport widths less than one are ok since they are real valued.
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);
        //image height and width are int versions so only aproximations of the values calculated by the aspect ratio 

        auto viewport_u = vec3(viewport_width, 0, 0);//horizontal vector of viewport
        auto viewport_v = vec3(0, -viewport_height, 0);//vertical vector of viewport

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel_origin = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

     ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel_origin
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(const ray& r, int depth, const hittable& world) {
        if(depth<=0)
            return color(0,0,0);//exceeded max bounce limit
        hit_record record;
        if (world.hit(r, interval(0.001, INF), record)) {//minimum accounts for float point inaccuracies causing bounces to be started offset from intersection
            ray scattered;
            color attenuation;
            if (record.mat->scatter(r, record, attenuation, scattered))//if the ray doesn't get absorbed
                return attenuation * ray_color(scattered, depth-1, world);
            
            return color(0,0,0);
        }
        vec3 unit_dir = unit_vector(r.direction());
        auto at = 0.5*(unit_dir.y()+1.0);
        return (1.0-at)*color(1.0, 1.0, 1.0) + at*color(0.5, 0.7, 1.0);
    }
};

#endif