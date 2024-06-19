#pragma once

#include "rtweekend.h"

class perlin {  // 柏林噪声
public:
    perlin() {  // 初始化，生成随机向量数组和排列数组
        randvec = new vec3[point_count];
        for (int i = 0; i < point_count; i++) {
            randvec[i] = unit_vector(vec3::random(-1,1));
        }

        // 随机化排列数组
        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() { // 释放内存
        delete[] randvec;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const point3& p) const {
        // 取出p的x、y和z坐标的小数部分，用作线性插值
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());

        // 计算出三个索引i、j和k。这些索引是通过将点p的x、y和z坐标分别向下取整，然后转换为整数并与255进行位与操作得到的。位与操作的目的是将结果限制在0到255的范围内。
        auto i = int(floor(p.x()));
        auto j = int(floor(p.y()));
        auto k = int(floor(p.z()));
        vec3 c[2][2][2];  // 存储噪声值的数组

        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[di][dj][dk] = randvec[
                        perm_x[(i+di) & 255] ^
                        perm_y[(j+dj) & 255] ^
                        perm_z[(k+dk) & 255]
                    ]; // 从randvec数组中取出一个随机向量作为噪声值。

        return perlin_interp(c, u, v, w); // 返回线性插值的结果
    }

    double turb(const point3& p, int depth) const { // 扰动函数
        auto accum = 0.0;   // 累加值
        auto temp_p = p;    
        auto weight = 1.0;  // 权重

        for (int i = 0; i < depth; i++) {   // 以衰减的权重累加噪声
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);  // 返回绝对值
    }

private:
    static const int point_count = 256; // 点的数量
    vec3* randvec;  // 随机向量数组
    int* perm_x;    // 排列数组x
    int* perm_y;    // 排列数组y
    int* perm_z;    // 排列数组z

    static int* perlin_generate_perm() {    // 生成排列数组
        auto p = new int[point_count];  // 生成一个长度为256的数组

        for (int i = 0; i < point_count; i++)   // 将数组初始化为0-255
            p[i] = i;

        permute(p, point_count);   // 随机排列数组

        return p;
    }

    static void permute(int* p, int n) {    // 随机排列数组
        for (int i = n-1; i > 0; i--) {
            int target = random_int(0, i);  // 随机交换的索引目标
            std::swap(p[i], p[target]);
        }
    }

    static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w) {
        // 计算三个方向上的Hermite插值
        auto uu = u*u*(3-2*u);
        auto vv = v*v*(3-2*v);
        auto ww = w*w*(3-2*w);
        auto accum = 0.0;

        for (int i=0; i < 2; i++)
            for (int j=0; j < 2; j++)
                for (int k=0; k < 2; k++) {
                    vec3 weight_v(u-i, v-j, w-k);   // 权重向量
                    accum += (i*uu + (1-i)*(1-uu))
                           * (j*vv + (1-j)*(1-vv))
                           * (k*ww + (1-k)*(1-ww))
                           * dot(c[i][j][k], weight_v); // 点乘
                }

        return accum; // [-1,1]
    }
};