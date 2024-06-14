#pragma once

class vec3 {
public:
    double e[3];

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

	// 返回x,y,z三个方向的值
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

	// 重载操作以适应vec3
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) { return *this *= 1/t; }
    double length() const { return sqrt(length_squared()); }
    double length_squared() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }
    bool near_zero() const { // 判断向量是否接近0向量
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }
    static vec3 random() { return vec3(random_double(), random_double(), random_double()); } // 随机生成一个vec3
    static vec3 random(double min, double max) { return vec3(random_double(min,max), random_double(min,max), random_double(min,max)); } // 随机生成一个vec3(有范围min-max)
};

// point3 只是 vec3 的别名，但对代码中的几何清晰度很有用。
using point3 = vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) { return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2]; }
inline vec3 operator+(const vec3& u, const vec3& v) { return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]); }
inline vec3 operator-(const vec3& u, const vec3& v) { return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]); }
inline vec3 operator*(const vec3& u, const vec3& v) { return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]); }
inline vec3 operator*(double t, const vec3& v) { return vec3(t*v.e[0], t*v.e[1], t*v.e[2]); }
inline vec3 operator*(const vec3& v, double t) { return t * v; }
inline vec3 operator/(const vec3& v, double t) { return (1/t) * v; }
inline double dot(const vec3& u, const vec3& v) { // 点乘
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}
inline vec3 cross(const vec3& u, const vec3& v) { // 叉乘
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
inline vec3 unit_vector(const vec3& v) { return v / v.length(); } // 返回一个单位向量

inline vec3 random_in_unit_disk() { // 在单位圆盘内随机生成一个点用于光圈模糊
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_in_unit_sphere() { // 在单位球内随机生成一个点用于漫反射, 只要点在圆内即可
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector() { return unit_vector(random_in_unit_sphere()); } // 随机生成一个单位向量

inline vec3 random_on_hemisphere(const vec3& normal) { // 使得生成的反射光线在反射表面法相的半球内
    vec3 on_unit_sphere = random_unit_vector();
    return dot(on_unit_sphere, normal) > 0.0 ? on_unit_sphere : -on_unit_sphere; // In the same hemisphere as the normal
}

inline vec3 reflect(const vec3& v, const vec3& n) { // 反射
    return v - 2*dot(v,n)*n; // dot(v,n)是v在n上的投影，2*dot(v,n)*n是v在n上的投影的两倍，v减去这个投影的两倍就是反射后的向量
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) { // 折射,uv是入射光线，n是法向量，etai_over_etat是折射率
    auto cos_theta = fmin(dot(-uv, n), 1.0); // cos_theta是uv与n的夹角
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n); // r_out_perp是折射光线在n上的投影
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n; // r_out_parallel是折射光线在n上的垂直于n的分量
    return r_out_perp + r_out_parallel; // 返回折射后的向量
}
