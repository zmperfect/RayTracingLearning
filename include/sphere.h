#pragma once

#include "hittable.h"

class sphere : public hittable {    // 球体类
public:
    sphere(const point3& center, double radius) : center(center), radius(fmax(0,radius)) {} // constructor(构造一个球体)

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {  //判断射线是否与球体相交
        // t^2d \cdot d - 2td \cdot (C-Q)+(C-Q)\cdot(C-Q)-r^2=0
        // 圆心C，半径r，射线起点Q，射线方向d，t为未知数(射线与球体的交点)
        // 简化 -2h=b=-2d\cdot(C-Q)
        // \frac{-b\pm \sqrt{b^2-4ac}}{2a}=\frac{-2h\pm \sqrt{(2h)^2-4ac}}{2a}=\frac{-h\pm \sqrt{h^2-ac}}{a}
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

        return true;
    }

private:
    point3 center;  // 球心坐标
    double radius;  // 半径
};