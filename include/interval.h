#pragma once

class interval { // 区间管理类
public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // 默认区间为空
    interval(double min, double max) : min(min), max(max) {}

    interval(const interval& a, const interval& b) { // 创建紧紧包围两个输入区间的区间
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    double size() const { return max - min; } // 返回区间大小
    bool contains(double x) const { return min <= x && x <= max; } // 判断x是否在区间内
    bool surrounds(double x) const { return min < x && x < max; } // 判断x是否在区间内部
    double clamp(double x) const { return (x < min) ? min : ((x > max) ? max : x); } // 将x限制在区间内

    interval expand(double delta) const { // 边界框添加一点内间隔
        auto padding = delta/2; // 间隔的一半
        return interval(min - padding, max + padding); // 返回扩展后的区间
    }

    static const interval empty, universe; // 空区间和全区间
};

const interval interval::empty    = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);