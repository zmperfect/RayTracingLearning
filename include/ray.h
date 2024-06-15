#pragma once

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction, double time = 0.0)
        : orig(origin), dir(direction), tm(time) {}

    ray(const point3& origin, const vec3& direction) 
        : orig(origin), dir(direction) {}

    const point3& origin() const  { return orig; }
    const vec3& direction() const { return dir; }

    double time() const { return tm; } // 返回光线所在的时间

    point3 at(double t) const { // Ray的表示：P(t) = A + tb
        return orig + t*dir;
    }

private:
    point3 orig; // Ray的起点
    vec3 dir;    // Ray的方向
    double tm;   // Ray的时间（光线自己所在的时刻）
};