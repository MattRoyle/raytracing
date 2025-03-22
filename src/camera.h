#ifndef CAMERA_H
#define CAMERA_H

#include "headers.h"

#include "hittable.h"
#include "pdf.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 16.0 / 9.0;//aspect ration is ideal ratio
    int image_width = 400;
    int samples_per_pixel = 10;//samples taken around each pixel for anti-aliasing
    int    max_depth         = 10;   // Maximum number of ray bounces into scene
    colour background_colour;     // Scene background colour
    double fov=90;
    point3 cam_center = point3(0,0,0);   // Point camera is looking from
    point3 look_point   = point3(0,0,-1);  // Point camera is looking at
    vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world, const hittable& lights) {
        initialize();
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";//formatting the .ppm file
        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;//writes to the console
            for (int i = 0; i < image_width; i++) {
                colour pixel_color(0,0,0);
                for (int s_j = 0; s_j < sqrt_spp; s_j++) {
                    for (int s_i = 0; s_i < sqrt_spp; s_i++) {
                        ray r = get_ray(i, j, s_i, s_j);
                        pixel_color += ray_colour(r, max_depth, world, lights);
                    }
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rDone.                 \n";
    }

  private:
    int    image_height;
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    int    sqrt_spp;             // Square root of number of samples per pixel
    double recip_sqrt_spp;       // 1 / sqrt_spp
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
        
        sqrt_spp = int(std::sqrt(samples_per_pixel));
        pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
        recip_sqrt_spp = 1.0 / sqrt_spp;

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

    ray get_ray(int i, int j, int s_i, int s_j) const {
        // Construct a camera ray originating from the focus disk and directed at randomly sampled
        // sampled point around the pixel location i, j for stratified sample square s_i, s_j.

        auto offset = sample_square_stratified(s_i, s_j);
        auto pixel_sample = pixel_origin
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? cam_center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square_stratified(int s_i, int s_j) const {
        // Returns the vector to a random point in the square sub-pixel specified by grid
        // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

        auto px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5;
        auto py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5;

        return vec3(px, py, 0);
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
    colour ray_colour(const ray& r, int depth, const hittable& world, const hittable& lights) {
        if(depth<=0)
            return colour(0,0,0);//exceeded max bounce limit so no more light is added

        hit_record record;

        // If the ray doesn't hit anything return the background colour
        if(!world.hit(r, interval(0.001, INF), record))
            return background_colour;

        ray scattered;
        colour attenuation;
        double pdf_value;
        colour emitted_color = record.mat->emitted(r, record, record.u, record.v, record.p);
        
        if (!record.mat->scatter(r, record, attenuation, scattered, pdf_value))//if the ray doesn't get absorbed
            return emitted_color;//ray was absorbed only return the emission colour
        
        auto pdf0 = make_shared<hittable_pdf>(lights, record.p);
        auto pdf1 = make_shared<cosine_pdf>(record.normal);
        mixture_pdf mixed_pdf(pdf0, pdf1);

        scattered = ray(record.p, mixed_pdf.generate(), r.time());
        pdf_value = mixed_pdf.value(scattered.direction());
        
        double scattering_pdf = record.mat->scattering_pdf(r, record, scattered);

        colour sample_colour = ray_colour(scattered, depth-1, world, lights);
        colour colour_from_scatter = (attenuation * scattering_pdf * sample_colour) / pdf_value; //pdf integration formula

        return emitted_color + colour_from_scatter;
    }
};

#endif