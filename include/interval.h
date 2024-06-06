#pragma once

class interval { // 区间管理类
public:
    double min, max;
    
    interval() : min(+infinity), max(-infinity) {} // 默认区间为空
    interval(double min, double max) : min(min), max(max) {}

    double size() const { return max - min; } // 返回区间大小
    bool contains(double x) const { return min <= x && x <= max; } // 判断x是否在区间内
    bool surrounds(double x) const { return min < x && x < max; } // 判断x是否在区间内部

    static const interval empty, universe; // 空区间和全区间
};

const interval interval::empty    = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);