#pragma once

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"
#include "Texture.h"

class constant_medium : public hittable {   // 恒定介质
public:
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<Texture> tex)
        : boundary(boundary), neg_inv_density(-1/density),
          phase_function(make_shared<isotropic>(tex))
    {}

    constant_medium(shared_ptr<hittable> boundary, double density, const color& albedo)
        : boundary(boundary), neg_inv_density(-1/density),
          phase_function(make_shared<isotropic>(albedo))
    {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {    // 判断射线是否与物体相交（相交是否有效,即含射线区间判断）
        // 调试时打印偶发样本。要启用此功能，请将 enableDebug 设置为 true。
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

        hit_record rec1, rec2;

        if (!boundary->hit(r, interval::universe, rec1))
            return false;

        if (!boundary->hit(r, interval(rec1.t+0.0001, infinity), rec2))
            return false;

        if (debugging) std::clog << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * log(random_double());

        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance = " <<  hit_distance << '\n'
                      << "rec.t = " <<  rec.t << '\n'
                      << "rec.p = " <<  rec.p << '\n';
        }

        rec.normal = vec3(1,0,0);  // arbitrary
        rec.front_face = true;     // also arbitrary
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override { return boundary->bounding_box(); }

private:
    shared_ptr<hittable> boundary;  // 边界
    double neg_inv_density; // 负介质密度
    shared_ptr<material> phase_function;    // 相位函数（isotropic各向同性）
};