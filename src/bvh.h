#ifndef BVH_H
#define BVH_H

#include "headers.h"

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable {
  public:
    // Build the bounding volume heirarchy using the hittable_objects list
    bvh_node(hittable_list list) : bvh_node(list.hittable_objects, 0, list.hittable_objects.size()) {
        // modifyable list variable has constructor lifetime
    }
    // Build the bounding volume heirarchy of the fitting the span of the source objects
    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
        bbox = aabb::empty;
        for (size_t object_index=start; object_index < end; object_index++)
            bbox = aabb(bbox, objects[object_index]->bounding_box());

        int axis = bbox.longest_axis();
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;//pick the comparator based on the chosen axis

        size_t object_span = end - start;//length of the list

        if (object_span == 1) {//1 object in the list, 
            childA = objects[start];// set childA to a leaf node
            childB = nullptr;// indicate no other child
        } else if (object_span == 2) {//two objects, doesn't matter if sorted
            childA = objects[start];//set both children to leaf nodes
            childB = objects[start+1];
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);//sort the objects along the dividing axis

            auto mid = start + object_span/2;//calculate the centre of the sorted list
            //create child nodes for the first and second halves of the sorted objects
            childA = make_shared<bvh_node>(objects, start, mid);
            childB = make_shared<bvh_node>(objects, mid, end);
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t))//does it intersect with the bounding box
            return false;

        bool hit_childA = childA->hit(r, ray_t, rec);

        if(childB==nullptr)
            return hit_childA;//doesn't check childB when only one child node

        bool hit_childB = childB->hit(r, interval(ray_t.min, hit_childA ? rec.t : ray_t.max), rec);

        return hit_childA || hit_childB;
    }
    // return the bounding box of the node
    aabb bounding_box() const override { return bbox; }

  private:
    shared_ptr<hittable> childA;
    shared_ptr<hittable> childB;
    aabb bbox;

    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index) {//takes the axis to compare the bounding boxes with
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;//compares the intervals
    }

    static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }
};

#endif