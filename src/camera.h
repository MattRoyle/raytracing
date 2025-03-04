#ifndef CAMERA_H
#define CAMERA_H

#include "headers.h"

#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 16.0 / 9.0;//aspect ration is ideal ratio
    int image_width = 400;
    int samples_per_pixel = 10;//samples taken around each pixel for anti-aliasing
    int    max_depth         = 10;   // Maximum number of ray bounces into scene

    double fov=90;
    point3 cam_center = point3(0,0,0);   // Point camera is looking from
    point3 look_point   = point3(0,0,-1);  // Point camera is looking at
    vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

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
    vec3   u, v, w;       // Camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius

    void initialize() {
       
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // calculate the image height, and ensure that it's at least 1.
        pixel_samples_scale = 1.0 / samples_per_pixel;

        camera_center =cam_center;
        
        auto theta = degrees_to_radians(fov);
        // Viewport widths less than one are ok since they are real valued.
        auto h = tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width)/image_height);
        //image height and width are int versions so only aproximations of the values calculated by the aspect ratio 

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(cam_center - look_point);//is oposite to focus point
        u = unit_vector(cross(vup, w));//u is horizontal viewport
        v = cross(w, u);//v is vertical viewport

        vec3 viewport_u = viewport_width*u;//horizontal viewport
        vec3 viewport_v = viewport_height * -v;//vertical viewport

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = camera_center - focus_dist*w - viewport_u/2 - viewport_v/2;
        pixel_origin = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

     ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the focus disk and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel_origin
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? cam_center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return cam_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
    color ray_color(const ray& r, int depth, const hittable& world) {
        if(depth<=0)
            return color(0,0,0);//exceeded max bounce limit
        hit_record record;
        if (world.hit(r, interval(0.001, INF), record)) {//minimum accounts for float point inaccuracies causing bounces to be started offset from intersection
            ray scattered;
            color attenuation;
            if (record.mat->scatter(r, record, attenuation, scattered))//if the ray doesn't get absorbed
                return attenuation * ray_color(scattered, depth-1, world);//recursive call
            
            return color(0,0,0);//ray was absorbed so return black
        }
        // Ray hits nothing
        vec3 ray_udirection = unit_vector(r.direction());
        auto at = 0.5*(ray_udirection.y()+1.0);
        //linear-interpolate the colour by "at": startValue = (1.0,1.0,1.0), endValue = (0.5,0.7,1.0) green?
        return (1.0-at)*color(1.0, 1.0, 1.0) + at*color(0.5, 0.7, 1.0);
    }
};

#endif