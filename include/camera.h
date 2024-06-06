#pragma once

#include "rtweekend.h"

#include "hittable.h"

class camera {
public:
    // Image
    double aspect_ratio   = 1.0;  // 纵横比
    int    image_width    = 100;  // 以像素为单位的图像宽度
    int samples_per_pixel = 10;   // 每个像素的采样次数

    void render(const hittable& world) { // 渲染图像
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;// 输出剩余扫描线
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0); // 像素颜色初始化为黑色
                for (int sample = 0; sample < samples_per_pixel; ++sample) { // 对每个像素进行多次采样
                    ray r = get_ray(i, j); // 获取射向点(i,j)射线
                    pixel_color += ray_color(r, world); // 累加颜色
                }
                write_color(std::cout, pixel_samples_scale * pixel_color); // 写入颜色（总采样的缩放）
            }  
        }
        std::clog << "\rDone.                 \n";// 输出完成
    }

private:
    int    image_height;   // 以像素为单位的图像高度
    double pixel_samples_scale; // 像素采样的缩放
    point3 center;         // 相机中心
    point3 pixel00_loc;    // 像素(0,0)的位置
    vec3   pixel_delta_u;  // 像素沿水平方向的偏移
    vec3   pixel_delta_v;  // 像素沿垂直方向的偏移

    void initialize() { // 初始化

        // Image
        image_height = int(image_width / aspect_ratio); // 计算图像高度且其至少为1
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel; // 计算像素采样的缩放

        center = point3(0, 0, 0); // 相机中心设置

        // 确定视口尺寸
        auto focal_length = 1.0; // 焦距
        auto viewport_height = 2.0; // 视口高度
        auto viewport_width = viewport_height * (double(image_width)/image_height); // 视口宽度

        // 计算横向和纵向视口边缘的矢量V_u,V_v
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // 计算像素间的水平和垂直三角矢量(本质是两个方向上像素的间隔)
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // 计算左上角像素的位置(边界像素的位置到边缘的距离仅为像素间隔的一半)
        auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(int i, int j) const { // 构建一条摄像机射线，射线从原点出发，指向像素位置(i,j)周围的随机采样点。
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = center; // Ray的起点为相机中心
        auto ray_direction = pixel_sample - ray_origin; // Ray的方向为从相机中心指向像素位置(i,j)周围的随机采样点

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const { // 返回指向 [-.5,-.5]-[+.5,+.5] 单位正方形中随机点的矢量。
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(const ray& r, const hittable& world) const {
        hit_record rec; // 记录射线与物体的交点信息
        if (world.hit(r, interval(0, infinity), rec)) { // 如果射线与某个物体相交，则返回该交点的颜色
            return 0.5 * (rec.normal + color(1,1,1));
        }

        vec3 unit_direction = unit_vector(r.direction());// 获取射线的方向，并将其转换为单位向量
        auto a = 0.5*(unit_direction.y() + 1.0);// 计算一个因子a，它的值在0到1之间。当射线的方向向上时（y分量为1），a的值为1；当射线的方向向下时（y分量为-1），a的值为0。
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);// 返回一个颜色，它是白色（1.0, 1.0, 1.0）和蓝色（0.5, 0.7, 1.0）的线性插值。当a为0时，返回白色；当a为1时，返回蓝色。当a在0和1之间时，返回两种颜色的混合。
    }
};