#pragma once

#include "rtweekend.h"

#include "hittable.h"

class quad : public hittable {
public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
    : Q(Q), u(u), v(v), mat(mat)    // 初始化，设置四边形的顶点Q，两个边向量u和v，以及材质
    {
        auto n = cross(u, v);   // 计算法向量
        normal = unit_vector(n); // normal是单位法向量
        D = dot(normal, Q); // Ax+By+Cz=D, D = -n·Q, n是法向量, Q是四边形起始点
        w = n / dot(n,n); // 存储法向量的倒数，用于加速计算

        set_bounding_box(); // 设置包围盒
    }

    virtual void set_bounding_box() {
        // 计算包围盒，先计算对角线的两个点组成的包围盒，然后计算其另外两个点组成的包围盒，最后合并两个包围盒
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto denom = dot(normal, r.direction()); // 计算射线方向与单位法向量的点积,考虑到normal是单位向量，所以这里计算的是射线方向与法向量的夹角的cos值，

        if (fabs(denom) < 1e-8) // 如果射线与四边形平行（即平面法向量与射线方向垂直），没有交点
            return false;

        // 计算射线与四边形的交点，Ax+By+Cz=D （即dot(n,v)=D）, R(t)=P+td, P是射线起点，d是射线方向，联合求解t
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) // 如果交点不在射线有效范围内
            return false;

        // 使用平面坐标确定命中点位于平面图形内
        auto intersection = r.at(t); // 计算交点
        vec3 planar_hitpt_vector = intersection - Q; // 计算交点相对于四边形起始点的向量p=P-Q 此处的P为Ray与四边形的交点
        auto alpha = dot(w, cross(planar_hitpt_vector, v)); // 计算向量p在u方向上的投影长度
        auto beta = dot(w, cross(u, planar_hitpt_vector)); // 计算向量p在v方向上的投影长度

        if (!is_interior(alpha, beta, rec))
            return false;

        // 射线击中 2D 形状；设置交点记录的其余部分并返回 true。
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        interval unit_interval = interval(0, 1); // (α,β)的单位区间
        // 根据平面坐标给出命中点，如果命中点位于基元之外，则返回 false，否则设置命中记录 UV 坐标并返回 true。

        if (!unit_interval.contains(a) || !unit_interval.contains(b)) // 如果交点不在四边形内
            return false;

        // 在四边形内部，设置UV坐标
        rec.u = a;
        rec.v = b;
        return true;
    }
    
private:
    point3 Q;   // 四边形的起始点(假设为左下角)
    vec3 u, v;  // Q的两个边向量
    vec3 w;  // 法向量的倒数，用于加速计算
    shared_ptr<material> mat; // 材质
    aabb bbox;  // 包围盒
    vec3 normal;  // 法向量
    double D; // Ax+By+Cz=D, D = -n·Q, n是法向量, Q是四边形起始点
};