#pragma once

#include "rtweekend.h"

#include "AABB.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable {
public:
    bvh_node(hittable_list list)
        : bvh_node(list.objects, 0, list.objects.size())
    {
        // 这里有一个 C++ 的微妙之处。这个构造函数（没有 span 索引）会创建一个隐式的可点击列表副本，我们将对其进行修改。复制列表的生命周期只持续到该构造函数退出为止。因为我们只需要持久化所生成的BVH。
    }

    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) { // 构造BVH树的结点，参数为物体列表，起始索引，结束索引
        // 构建源对象跨度的包围盒
        bbox = aabb::empty; // 初始化包围盒为空
        for (size_t object_index=start; object_index < end; object_index++) // 遍历所有物体,更新包围盒
            bbox = aabb(bbox, objects[object_index]->bounding_box());

        int axis = bbox.longest_axis(); // 获取包围盒最长的轴

        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare; // 根据坐标轴选择包围盒比较函数

        size_t object_span = end - start;   // 物体跨度

        if (object_span == 1) { // 如果只有一个物体
            left = right = objects[start]; // 左右子树都是这个物体
        } else if (object_span == 2) {  // 如果有两个物体
            left = objects[start];  // 左子树是第一个物体
            right = objects[start+1];   // 右子树是第二个物体
        } else {    // 如果有多个物体
            std::sort(objects.begin() + start, objects.begin() + end, comparator);  // 对物体（按照包围盒的最左边的位置）进行排序

            // 递归构建左右子树
            auto mid = start + object_span/2;   // 中间位置
            left = make_shared<bvh_node>(objects, start, mid);  // 递归构建左子树
            right = make_shared<bvh_node>(objects, mid, end);   // 递归构建右子树
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override { // 判断射线是否与BVH树相交
        if (!bbox.hit(r, ray_t))    // 如果射线与包围盒不相交，直接返回false
            return false;

        bool hit_left = left->hit(r, ray_t, rec);   // 判断射线是否与左子树相交
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec); // 判断射线是否与右子树相交

        return hit_left || hit_right; // 返回左右子树是否有一个相交
    }

    aabb bounding_box() const override { return bbox; } // 返回包围盒

private:
    shared_ptr<hittable> left; // 左子树
    shared_ptr<hittable> right;// 右子树
    aabb bbox; // 包围盒

    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index) { // 比较两个物体在某个坐标轴上的包围盒
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index); // 获取a的包围盒在axis_index轴上的区间
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index); // 获取b的包围盒在axis_index轴上的区间
        return a_axis_interval.min < b_axis_interval.min; // 返回两个区间的最小值
    }

    static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 0); } // 比较x轴上的包围盒

    static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 1); } // 比较y轴上的包围盒

    static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 2); } // 比较z轴上的包围盒
};