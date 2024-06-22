#pragma once

#include "rtweekend.h"

class aabb {
public:
    interval x, y, z; // 三个坐标轴方向上的区间

    aabb() {} // 默认AABB为空，即没有间隔

    aabb(const interval& x, const interval& y, const interval& z)
        : x(x), y(y), z(z)
        {
            pad_to_minimums(); // 边界框填充以避免某个轴的宽度为零
        }

    aabb(const point3& a, const point3& b) {
        // 将 a 和 b 两点视为边界框的极值，因此我们不需要特定的最小/最大坐标顺序。
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    aabb(const aabb& box0, const aabb& box1) { // 创建紧紧包围两个输入AABB的AABB
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);

        pad_to_minimums(); // pad to minimum
    }

    const interval& axis_interval(int n) const { // 返回第n个坐标轴的区间
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const { // 判断射线是否与AABB相交
        const point3& ray_orig = r.origin();    // 射线起点
        const vec3&   ray_dir  = r.direction(); // 射线方向

        for (int axis = 0; axis < 3; axis++) {  // 遍历三个坐标轴，判断射线是否与AABB相交
            // 计算射线与AABB的交点（其实求的是t，对应P(t)=Q+td）
            const interval& ax = axis_interval(axis); // 获取第axis个坐标轴的区间
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            // 确保t0<t1，然后更新ray_t的区间
            ray_t.min = std::max(ray_t.min, std::min(t0, t1));
            ray_t.max = std::min(ray_t.max, std::max(t0, t1));

            if (ray_t.max <= ray_t.min) // 如果ray_t区间为空，则返回false
                return false;
        }
        return true;
    }

    int longest_axis() const { // 返回AABB最长的轴
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    static const aabb empty, universe; // 空AABB和全AABB

private:
    void pad_to_minimums() {
        // 调整 AABB，使任何一边都不比某个`delta`小，必要时进行填充。

        double delta = 0.0001;
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }
};

const aabb aabb::empty    = aabb(interval::empty,    interval::empty,    interval::empty);    // 空AABB
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe); // 全AABB