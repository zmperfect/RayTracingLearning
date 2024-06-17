#pragma once

#include "rtweekend.h"

#include "Texture.h"

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
    lambertian(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override { //是否散射
        auto scatter_direction = rec.normal + random_unit_vector(); //接触点的单位法向量+随机生成的单位向量生成一个散射向量 （即散射方向）

        if (scatter_direction.near_zero()) //如果生成的向量接近0向量
            scatter_direction = rec.normal; //则将散射方向设置为法向量

        scattered = ray(rec.p, scatter_direction, r_in.time()); //生成一条射线
        attenuation = tex->value(rec.u, rec.v, rec.p); // 反射1个单位光线后的衰减
        return true;
    }

private:
    shared_ptr<Texture> tex; //纹理
    color albedo;// Albedo 参数控制着表面的基色
};

class metal : public material {
public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override { //是否散射
        vec3 reflected = reflect(r_in.direction(), rec.normal); //反射方向
        reflected += fuzz * random_in_unit_sphere(); //反射方向+模糊度(模糊球的半径) * 单位球内随机生成的点
        scattered = ray(rec.p, reflected, r_in.time()); //生成一条射线
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0); //如果反射光线与法向量的点积大于0，则返回true
    }

private:
    color albedo; // Albedo 参数控制着表面的基色
    double fuzz; //模糊度(实际上是反射光线的扩散程度(模糊球的半径))
};

class dielectric : public material { //介质(有折射)
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {} 

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override { //是否折射
        attenuation = color(1.0, 1.0, 1.0); //衰减
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index; //折射率（看接触面是否是入射光接触的表面，来决定是否哪个介质为入射光线所在）

        vec3 unit_direction = unit_vector(r_in.direction()); //入射光线的单位向量
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0); //入射光线与法向量的点积
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta); //入射光线与法向量的点积的平方根

        bool cannot_refract = ri * sin_theta > 1.0; //是否不能折射
        vec3 direction;//方向

        if (cannot_refract || reflectance(cos_theta, ri) > random_double()) //如果不能折射或反射率大于随机数
            direction = reflect(unit_direction, rec.normal); //反射方向
        else
            direction = refract(unit_direction, rec.normal, ri); //折射方向

        scattered = ray(rec.p, direction, r_in.time()); // 生成一条光线
        return true;
    }

private:
    double refraction_index; //真空或空气中的折射率，或材料的折射率与封闭介质的折射率之比
    static double reflectance(double cosine, double refraction_index) { // 计算反射率
        // 使用克里斯托夫·施利克近似公式
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};