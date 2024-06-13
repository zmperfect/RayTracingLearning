#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>


// C++ Std Usings

using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// 常量

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// 实用函数

inline double degrees_to_radians(double degrees) {  // 角度转弧度
    return degrees * pi / 180.0;
}

inline double random_double() { // 生成[0,1)之间的随机数
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) { // 生成[min,max)之间的随机数
    return min + (max-min)*random_double();
}

// 常用头文件

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
