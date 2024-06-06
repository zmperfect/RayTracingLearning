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