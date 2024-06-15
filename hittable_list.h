#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "headers.h"
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
    }

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_tmax;

        for (const auto& object : hittable_objects) {
            if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {//subtype polymorthism for abstract class hittable
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};

#endif