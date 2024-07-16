#pragma once

#include "rtweekend.h"

#include "AABB.h"

class material; // 材质

class hit_record {  // 记录射线与物体的交点信息
public:
    point3 p; // 交点坐标
    vec3 normal; //法线
    shared_ptr<material> mat; // 材质
    double t; // 交点的t值 Ray的表示：P(t) = A + tb
    double u, v; // 纹理坐标(u,v)
    bool front_face; // 是否是正面

    void set_face_normal(const ray& r, const vec3& outward_normal) { // 设置面法线
        // 设置交点的法线和正面
        // NOTE: 参数 `outward_normal` 假定为单位长度。
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0; // 判断射线是否与物体相交（相交是否有效,即含射线区间判断）

    virtual aabb bounding_box() const = 0; // 返回物体的包围盒
};

class translate : public hittable { // 平移物体
public:
    translate(shared_ptr<hittable> object, const vec3& offset)
        : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // 将射线向后移动偏移量
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        // 确定偏移射线上是否存在交点（如果存在，交点在哪里）
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // 将交叉点向前移动偏移量
        rec.p += offset;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};

class rotate_y : public hittable {  // 绕y轴旋转物体
public:
    rotate_y(shared_ptr<hittable> object, double angle) : object(object) {  // 物体object绕y轴旋转，angle为角度
        auto radians = degrees_to_radians(angle);   // 角度转弧度
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        // 计算包围盒
        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {    // 判断射线是否与物体相交（相交是否有效,即含射线区间判断）
        // 将光线从世界空间切换到对象空间
        auto origin = r.origin();
        auto direction = r.direction();

        // 光线原点和方向的旋转
        origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
        origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

        direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
        direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

        ray rotated_r(origin, direction, r.time());

        // 确定对象空间中是否存在交点（如果存在，交点在哪里）
        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // 将交点从对象空间移至世界空间
        auto p = rec.p;
        p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

        // 将法线从对象空间移至世界空间
        auto normal = rec.normal;
        normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> object;    // 物体
    double sin_theta;   // sin(θ)
    double cos_theta;   // cos(θ)
    aabb bbox;  // 包围盒
};