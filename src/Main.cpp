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
	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.50); // 折射率为1.50
	auto material_bubble = make_shared<dielectric>(1.00 / 1.50); // 玻璃球内部的折射率
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

	// 添加物体（附加材质）
    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left)); // 左侧的玻璃球外侧
	world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble)); // 左侧的玻璃球内侧
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

	// Camera
	camera cam; // 相机

	cam.aspect_ratio 		= 16.0 / 9.0; // 设置纵横比
	cam.image_width  		= 400; // 设置图像宽度
	cam.samples_per_pixel 	= 100; // 设置每个像素的采样次数
	cam.max_depth         	= 50; // 设置递归深度

	// Render
    cam.render(world);
}
