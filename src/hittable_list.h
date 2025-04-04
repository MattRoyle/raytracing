#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "headers.h"

#include "aabb.h"
#include "hittable.h"

#include <vector>

class hittable_list : public hittable {
  public:
    std::vector<shared_ptr<hittable>> hittable_objects;// vector of ptrs to hittable objects

    //constructors
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { hittable_objects.clear(); }

    void add(shared_ptr<hittable> object) {
        hittable_objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());//recalcutes the bounding box
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : hittable_objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {//only alows hits if they are closer than the current closest
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
    aabb bounding_box() const override { return bbox; }//get the bounding box of the objects in the list

  private:
    aabb bbox;
};

#endif