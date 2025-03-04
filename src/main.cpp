#include "headers.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"
#include <chrono>
void quads() {
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(colour(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(colour(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(colour(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(colour(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(colour(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<triangle>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<annulus>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0),0.6, right_blue));
    world.add(make_shared<ellipse>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fov     = 80;
    cam.cam_center = point3(0,0,9);
    cam.look_point   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;
    cam.background_colour = colour(.7, .8, 1.0);

    cam.render(world);
}

void bouncing_spheres() {
    hittable_list world;
    auto checker = make_shared<checker_texture>(0.32, colour(.2, .3, .1), colour(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = colour::random() * colour::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fov = 90;
    cam.cam_center = point3(13,2,3);
    cam.look_point = point3(0,0,0);
    cam.vup = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;
    cam.background_colour = colour(.7, .8, 1.0);
    //render the world 
    cam.render(world);
}

void checkered_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, colour(.2, .3, .1), colour(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fov     = 20;
    cam.cam_center = point3(13,2,3);
    cam.look_point   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;
    cam.background_colour = colour(.7, .8, 1.0);

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 3;
    cam.max_depth         = 10;

    cam.fov     = 20;
    cam.cam_center = point3(0,0,12);
    cam.look_point   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;
    cam.background_colour = colour(.7, .8, 1.0);

    cam.render(hittable_list(globe));
}
void perlin_spheres() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fov     = 20;
    cam.cam_center = point3(13,2,3);
    cam.look_point   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;
    cam.background_colour = colour(.7, .8, 1.0);

    cam.render(world);
}

void simple_light() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);//perlin noise texture
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(colour(4,4,4));//diffuse light material
    world.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
    world.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background_colour = colour(0,0,0);

    cam.fov     = 20;
    cam.cam_center = point3(26,3,6);
    cam.look_point   = point3(0,2,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}
void cornell_box() {
    hittable_list world;

    auto red   = make_shared<lambertian>(colour(.65, .05, .05));
    auto white = make_shared<lambertian>(colour(.73, .73, .73));
    auto green = make_shared<lambertian>(colour(.12, .45, .15));
    auto light = make_shared<diffuse_light>(colour(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    world.add(box1);

    shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    world.add(box2);

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background_colour = colour(0,0,0);

    cam.fov        = 40;
    cam.cam_center = point3(278, 278, -800);
    cam.look_point = point3(278, 278, 0);
    cam.vup        = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}
#include <iomanip>
int main() {
    auto start = std::chrono::high_resolution_clock::now();
    switch (3) {
        case 1:  bouncing_spheres();  break;
        case 2:  checkered_spheres(); break;
        case 3:  earth();             break;
        case 4:  perlin_spheres();    break;
        case 5:  quads();             break;
        case 6:  simple_light();      break;
        case 7:  cornell_box();       break;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()/1000000.f;
    
    std::clog << "Time taken: "<< std::fixed << std::setprecision(3) <<duration << "s\n";
}