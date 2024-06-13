#pragma once

#include "rtweekend.h"

class hit_record; //记录射线与物体的交点信息

class material { //材质
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const { //是否散射
        return false;
    }
};

class lambertian : public material { //郎伯反射（漫反射）
public:
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override { //是否散射
        auto scatter_direction = rec.normal + random_unit_vector(); //接触点的单位+法向量随机生成一个单位向量 （即散射方向）

        if (scatter_direction.near_zero()) //如果生成的向量接近0向量
            scatter_direction = rec.normal; //则将散射方向设置为法向量

        scattered = ray(rec.p, scatter_direction); //生成一条射线
        attenuation = albedo; // 反射率
        return true;
    }

private:
    color albedo;// 反射率
};

class metal : public material {
public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override { //是否散射
        vec3 reflected = reflect(r_in.direction(), rec.normal); //反射方向
        reflected += fuzz * random_in_unit_sphere(); //反射方向+模糊度(模糊球的半径) * 单位球内随机生成的点
        scattered = ray(rec.p, reflected); //生成一条射线
        attenuation = albedo; //反射率
        return (dot(scattered.direction(), rec.normal) > 0); //如果反射光线与法向量的点积大于0，则返回true
    }

private:
    color albedo; // 反射率
    double fuzz; //模糊度(实际上是反射光线的扩散程度(模糊球的半径))
};