#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"

class camera {
public:
    // Image
    double aspect_ratio   = 1.0;  // 纵横比
    int    image_width    = 100;  // 以像素为单位的图像宽度
    int samples_per_pixel = 10;   // 每个像素的采样次数
    int max_depth         = 10;   // 递归深度(进入场景的最大射线反弹次数)
    int channels = 3; // 每个像素的通道数，对于RGB图像是3
    unsigned char* data = nullptr;  // 图像数据
    color background;               // 场景背景颜色

    // Camera
    double vfov = 90;                // 垂直视角（视野）
    point3 lookfrom = point3(0,0,0); // 相机点(相机位置)
    point3 lookat = point3(0,0,-1);  // 观察点(相机看向的位置)
    vec3   vup = vec3(0,1,0);        // 相机的上方向(这样相机可以绕lookfrom-lookat的轴向旋转)

    // 焦平面相关（可计算光圈大小）defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2))
    double defocus_angle = 0; // 圆锥体的角度，其顶点位于视口中心，底部（散焦盘）位于相机中心，可以用来换算焦平面的半径（即光圈大小） 
    double focus_dist = 10;   // 焦距，相机中心到完美焦平面（视口在完美焦平面上(其上的图像不会被模糊)）的距离

    void render(const hittable& world) { // 渲染图像
        initialize();

        data = new unsigned char[image_width * image_height * channels]; // 创建图像数据缓冲区
        std::cout << "Parameters\n" << image_width << ' ' << image_height << ' ' << channels << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;// 输出剩余扫描线
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0); // 像素颜色初始化为黑色
                for (int sample = 0; sample < samples_per_pixel; ++sample) { // 对每个像素进行多次采样
                    ray r = get_ray(i, j); // 获取射向点(i,j)射线
                    pixel_color += ray_color(r, max_depth, world); // 累加颜色
                }
                int pixelIndex = (j * image_width + i) * channels; // 获取当前待写入像素索引
                write_color(pixelIndex, data, pixel_samples_scale * pixel_color); // 写入颜色（总采样的缩放）
            }
        }
        std::clog << "\rDone.                 \n";// 输出完成

        // 使用stbi_write_png将图像数据写入文件
        stbi_write_png("..//output//output.png", image_width, image_height, channels, data, image_width * channels);

        // 清理资源
        delete[] data;
    }

private:
    int    image_height;   // 以像素为单位的图像高度
    double pixel_samples_scale; // 像素采样的缩放
    point3 center;         // 相机中心
    point3 pixel00_loc;    // 像素(0,0)的位置
    vec3   pixel_delta_u;  // 像素沿水平方向的偏移
    vec3   pixel_delta_v;  // 像素沿垂直方向的偏移
    vec3   u, v, w;        // 相机坐标系的三个基向量(u指向右，v指向上，w指向观察点的反方向)
    vec3 defocus_disk_u;   // 焦平面上水平方向的向量
    vec3 defocus_disk_v;   // 焦平面上垂直方向的向量

    void initialize() { // 初始化

        // Image
        image_height = int(image_width / aspect_ratio); // 计算图像高度且其至少为1
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel; // 计算像素采样的缩放

        center = lookfrom; // 相机中心设置

        // 确定视口尺寸
        auto theta = degrees_to_radians(vfov); // 视野角度(弧度)
        auto h = tan(theta/2); // 视野角度的一半
        auto viewport_height = 2 * h * focus_dist; // 视口高度（此处2*h计算的是根据视野角度计算的视口高度，其与焦平面上的视口高度成等比例）
        auto viewport_width = viewport_height * (double(image_width)/image_height); // 视口宽度

        // 计算相机坐标系的三个基向量(u指向右，v指向上，w指向观察点的反方向)
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // 计算横向和纵向视口边缘的矢量V_u,V_v
        vec3 viewport_u = viewport_width * u;  // 沿着视口水平方向的矢量
        vec3 viewport_v = viewport_height * -v; // 沿着视口垂直方向的矢量(注意这里是-v，因为v（相机坐标系）是指向上的，而视口的垂直方向是向下的)

        // 计算像素间的水平和垂直三角矢量(本质是两个方向上像素的间隔)
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // 计算左上角像素的位置(边界像素的位置到边缘的距离仅为像素间隔的一半)
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // 计算焦平面上的基向量
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2)); // 计算焦平面的半径
        defocus_disk_u = u * defocus_radius; // 焦平面上水平方向的向量
        defocus_disk_v = v * defocus_radius; // 焦平面上垂直方向的向量
    }

    ray get_ray(int i, int j) const { // 构建一条摄像机光线，从散焦盘出发，指向像素位置 i、j 周围的随机采样点
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample(); // 如果焦平面模糊角度为0（即焦平面半径（光圈大小）为0），则光线从相机中心发出，否则从焦平面上的随机点发出
        auto ray_direction = pixel_sample - ray_origin; // Ray的方向为从相机中心指向像素位置(i,j)周围的随机采样点
        auto ray_time = random_double(); // 随机生成光线的时间

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square() const { // 返回指向 [-.5,-.5]-[+.5,+.5] 单位正方形中随机点的矢量。
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const { // 返回焦平面上的随机点
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0) // 如果超过光线反射的递归深度，则返回黑色
            return color(0,0,0);

        hit_record rec; // 记录射线与物体的交点信息

        // 如果ray没有与任何物体相交，则返回背景颜色
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered; // 散射的射线
        color attenuation; // 衰减
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p); // 获取发射的颜色

        // 如果材质发生散射，则返回散射的射线和衰减
        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth-1, world); 

        return color_from_emission + color_from_scatter;
    }
};