#pragma once

#include "hittable.h"

class sphere : public hittable {    // 球体类
public:
    // 静止球体
    sphere(const point3& center, double radius, shared_ptr<material> mat)
        : center1(center), radius(fmax(0,radius)), mat(mat), is_moving(false) {}

    // 运动球体
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat)
        : center1(center1), radius(fmax(0,radius)), mat(mat), is_moving(true)
    {
        center_vec = center2 - center1; // 计算球心运动方向
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {  //判断射线是否与球体相交
        // t^2d \cdot d - 2td \cdot (C-Q)+(C-Q)\cdot(C-Q)-r^2=0
        // 圆心C，半径r，射线起点Q，射线方向d，t为未知数(射线与球体的交点)
        // 简化 -2h=b=-2d\cdot(C-Q)
        // \frac{-b\pm \sqrt{b^2-4ac}}{2a}=\frac{-2h\pm \sqrt{(2h)^2-4ac}}{2a}=\frac{-h\pm \sqrt{h^2-ac}}{a}
        point3 center = is_moving ? sphere_center(r.time()) : center1; // 计算球心位置
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = sqrt(discriminant);

        // 找出位于可接受范围内的最近的根。
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        // 记录交点信息
        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    point3 center1;  // 球心坐标
    double radius;  // 半径
    shared_ptr<material> mat; // 材质
    bool is_moving; // 是否是运动球体
    vec3 center_vec; // 球心运动方向

    point3 sphere_center(double time) const {
        // 根据时间从中心 1 线性插值到中心 2，其中 t=0 表示中心 1，t=1 表示中心 2。
        return center1 + time*center_vec;
    }
};