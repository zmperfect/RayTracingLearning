#pragma once

#include "interval.h"
#include "vec3.h"

using color = vec3;

inline double linear_to_gamma(double linear_component) { // 线性颜色转伽马颜色
    if (linear_component > 0)
        return sqrt(linear_component);

    return 0;
}

void write_color(std::ostream& out, const color& pixel_color) { // 写入每个坐标的颜色
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // 为伽玛 2 应用线性伽玛变换
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999); //限制颜色值在0-1之间
    int rbyte = static_cast<int>(255.999 * intensity.clamp(r));  
    int gbyte = static_cast<int>(255.999 * intensity.clamp(g));  
    int bbyte = static_cast<int>(255.999 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}