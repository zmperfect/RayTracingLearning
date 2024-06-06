#pragma once

#include "hittable.h"

class hittable_list : public hittable { // 用于存储所有命中（与Ray相交）的物体
public:
    hittable_list() {}
    hittable_list(std::shared_ptr<hittable> object) { add(object); } // 添加物体

    void clear() { objects.clear(); } // 清空物体
    void add(std::shared_ptr<hittable> object) { objects.push_back(object); } // 将物体添加到列表中

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {  // 判断射线是否与物体相交
        hit_record temp_rec;    // 临时记录
        bool hit_anything = false;  // 是否有物体被击中
        auto closest_so_far = ray_t.max; // Ray的最远有效t

        for (const auto& object : objects) { // 遍历所有物体，检查是否有物体被击中
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

private:
    std::vector<std::shared_ptr<hittable>> objects;
};