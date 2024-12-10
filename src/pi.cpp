// #include "rtweekend.h"

// #include <iostream>
// #include <iomanip>

// int main()
// {
//     std::cout << std::fixed << std::setprecision(12);   // std::fixed用于用于将浮点数的输出格式设置为固定小数点表示法，std::setprecision(12)用于设置浮点数的输出精度

//     int inside_circle = 0;
//     int inside_circle_stratified = 0;
//     int sqrt_N = 1000;

//     for (int i = 0; i < sqrt_N; i++) {
//         for (int j = 0; j < sqrt_N; j++) {
//             // 常规采样
//             auto x = random_double(-1,1);
//             auto y = random_double(-1,1);
//             if (x*x + y*y < 1)
//                 inside_circle++;

//             // 分层采样
//             x = 2*((i + random_double()) / sqrt_N) - 1;
//             y = 2*((j + random_double()) / sqrt_N) - 1;
//             if (x*x + y*y < 1)
//                 inside_circle_stratified++;
//         }
//     }

//     std::cout
//         << "Regular    Estimate of Pi = "
//         << (4.0 * inside_circle) / (sqrt_N*sqrt_N) << '\n'
//         << "Stratified Estimate of Pi = "
//         << (4.0 * inside_circle_stratified) / (sqrt_N*sqrt_N) << '\n';
// }