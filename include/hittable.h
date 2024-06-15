#pragma once

class material; // 材质

class hit_record {  // 记录射线与物体的交点信息
public:
    point3 p; // 交点坐标
    vec3 normal; //法线
    shared_ptr<material> mat; // 材质
    double t; // 交点的t值 Ray的表示：P(t) = A + tb
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
};