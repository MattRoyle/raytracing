#include "color.h"
#include "vec3.h"
#include <iostream>
#include "ray.h"
template <typename T>
T lerp(T start, T end, double at){
    return (1.0-at)*start + at*end;
}

double hit_sphere(const point3& sphere_center, double sphere_radius, const ray& r){
    vec3 offsetCenter= sphere_center - r.origin();// sphere's position relative to the ray start
    auto a = dot(r.direction(),r.direction()); //derived value for a in quadratic to find the t intercections with the sphere
    auto b = -2.0 * dot(r.direction(), offsetCenter);
    auto c = dot(offsetCenter, offsetCenter) - sphere_radius *sphere_radius;
    auto discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0) // three cases: 0 means 1 solution (intersection), positive means 2 solutions, negative means no real solutions
    {
        return -1.0;
    }else{
        return (-b - sqrt(discriminant)/(2.0*a));//return the (negative) solution
    }
}

color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0,0,-1), 0.5, r);//sphere at -1 on z axis
    if (t>0.0){
        vec3 N = unit_vector(r.at(t) - vec3(0,0,-1));// intersection point minus the circle center
        return 0.5*color(N.x()+1, N.y()+1, N.z()+1);// N is between -1 and 1 so we add 1 then multiply by 0.5 to get 0 to 1
    }
       

    vec3 unit_dir = unit_vector(r.direction());
    auto at = 0.5*(unit_dir.y()+1.0);
    return lerp(color(1.0,1.0,1.0), color(0.5,0.7,1.0), at);
}

int main() {

    // Image
    auto aspect_ratio = 16.0 / 9.0;//aspect ration is ideal ratio
    int image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Viewport widths less than one are ok since they are real valued.
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width)/image_height);
    //image height and width are int versions so only aproximations of the values calculated by the aspect ratio 

    // Camera
    auto focal_length = 1.0; //inital value of orthagonal dist from viewport
    auto camera_center = point3(0, 0, 0);

    auto viewport_u = vec3(viewport_width, 0, 0);//horizontal vector of viewport
    auto viewport_v = vec3(0, -viewport_height, 0);//vertical vector of viewport

    auto pixel_delta_u = viewport_u / image_width;//horizontal distance between pixels
    auto pixel_delta_v = viewport_v / image_height;// vertical distance between pixels

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel_origin = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;//writes the lines remaining flushing the current
        for (int i = 0; i < image_width; i++) {
            point3 pixel_center = pixel_origin + (i*pixel_delta_u)+(j*pixel_delta_v);
            vec3 ray_dir = pixel_center - camera_center;
            ray ray(camera_center,ray_dir);

            color pixel_color = ray_color(ray);// creates a colour using the x and y positions
            write_color(std::cout, pixel_color);//writes to the out
        }
    }
    std::clog << "\rDone.                 \n";
}
