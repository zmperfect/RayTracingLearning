#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>


// C++ Std Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// 常量

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// 实用函数

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// 常用头文件

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
