#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

int main() {
	// World
	hittable_list world; // 世界中的物体与光线相交

	// 添加材质
	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5)); // 地面材质(郎伯反射)
	// 添加物体
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));// 地面

	// 随机生成小球
	for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();	// 随机生成一个数用于选择材质
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());	// 随机生成小球的中心

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {	// 如果小球的中心不在(4,0.2,0)附近
                shared_ptr<material> sphere_material;	// 小球的材质

                if (choose_mat < 0.8) {
                    // 漫反射
                    auto albedo = color::random() * color::random();	// 随机生成一个颜色
                    sphere_material = make_shared<lambertian>(albedo);	// 创建一个漫反射材质
                    auto center2 = center + vec3(0, random_double(0,.5), 0);	// 随机生成一个小球的中心
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material)); // 添加一个运动球体
                } else if (choose_mat < 0.95) {
                    // 金属材质
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // 玻璃材质
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

	// 添加三个大球（介质（玻璃）、漫反射、金属）
	auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	// Camera
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0; // 纵横比
    cam.image_width       = 400;	// 图像宽度
    cam.samples_per_pixel = 100;	// 每个像素的采样次数
    cam.max_depth         = 50;		// 递归深度（进入场景的最大反弹次数）

	// 相机位置
    cam.vfov     = 20;				// 垂直视角（视野）
    cam.lookfrom = point3(13,2,3);	// 相机点(相机位置)
    cam.lookat   = point3(0,0,0);	// 观察点(相机看向的位置)
    cam.vup      = vec3(0,1,0);		// 相机的上方向(这样相机可以绕lookfrom-lookat的轴向旋转)

	// 焦平面相关（可计算光圈大小）defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2))
    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

	// Render
    cam.render(world);
}
