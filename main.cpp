#include "headers.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

color ray_color(const ray& r, const hittable& world) {
    hit_record record;
    if (world.hit(r, 0, INF, record)) {
        return 0.5 * (record.normal + color(1,1,1));
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


    // World

    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));


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

            color pixel_color = ray_color(ray, world);// creates a colour using the x and y positions
            write_color(std::cout, pixel_color);//writes to the out
        }
    }
    std::clog << "\rDone.                 \n";
}
