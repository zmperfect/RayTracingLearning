#pragma once

#include "rtweekend.h"

#include "perlin.h"
#include "rtw_stb_image.h"

class Texture { // 纹理
public:
    virtual ~Texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0; // 根据纹理坐标返回纹理颜色
};

class solid_color : public Texture { // 恒定的颜色纹理
public:
    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue)
        :solid_color(color(red,green,blue)) {}

    color value(double u, double v, const point3& p) const override {
        return albedo;
    }

private:
    color albedo; //Albedo 参数控制着表面的基色
};

class checker_texture : public Texture { // 棋盘纹理
public:
    checker_texture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)    // 棋盘纹理的缩放比例，偶数纹理，奇数纹理
        : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(double scale, const color& c1, const color& c2) // 棋盘纹理的缩放比例，偶数纹理颜色，奇数纹理颜色
        : inv_scale(1.0 / scale),
        even(make_shared<solid_color>(c1)),
        odd(make_shared<solid_color>(c2))
    {}

    color value(double u, double v, const point3& p) const override { // 根据纹理坐标返回纹理颜色
        // 纹理坐标映射到棋盘纹理上
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0; // 判断是否是偶数纹理

        return isEven ? even->value(u, v, p) : odd->value(u, v, p); // 返回偶数纹理或奇数纹理的颜色
    }

private:
    double inv_scale; // 棋盘纹理的缩放比例
    shared_ptr<Texture> even;   // 偶数纹理
    shared_ptr<Texture> odd;    // 奇数纹理
};

class image_texture : public Texture { // 图像纹理
public:
    image_texture(const char* filename) : image(filename) {} // 从文件名加载图像

    color value(double u, double v, const point3& p) const override {
        // 如果没有纹理数据，则返回纯青色作为调试辅助
        if (image.height() <= 0) return color(0,1,1);

        // 将输入的纹理坐标限制在[0,1] x [1,0]之间
        u = interval(0,1).clamp(u);
        v = 1.0 - interval(0,1).clamp(v);  // 将 V 翻转到图像坐标

        // 纹理坐标映射到图像上
        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i,j); // 获取(i,j)处像素数据

        // 将像素数据转换为颜色(原来的像素数据是[0,255]范围内的整数值，转换层[0,1]范围内的浮点数值)
        auto color_scale = 1.0 / 255.0;
        return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }
private:
    rtw_image image; // 图像
};

class noise_texture : public Texture { // 噪声纹理
public:
    noise_texture() {}

    noise_texture(double scale = 1) : scale(scale) {}

    color value(double u, double v, const point3& p) const override {
        // 返回噪声纹理的颜色
        return color(0.5, 0.5, 0.5) * (1 + sin(scale * p.z() + 10 * noise.turb(p, 7))); // 进行扰动，同时调整频率
    }
private:
    perlin noise; // 柏林噪声
    double scale; // 噪声纹理的缩放比例
};