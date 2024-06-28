#include "rtweekend.h"

#include "BVH.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "Quad.h"
#include "sphere.h"
#include "texture.h"

void bouncing_spheres() { // 反弹小球的场景
	// World
	hittable_list world; // 世界中的物体与光线相交

	// 添加地面
	auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9)); // 棋盘纹理（当成材质传入1）
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker))); // 添加一个地面

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

    world = hittable_list(make_shared<bvh_node>(world)); // 构建BVH树

	// Camera
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0; // 纵横比
    cam.image_width       = 400;	// 图像宽度
    cam.samples_per_pixel = 100;	// 每个像素的采样次数
    cam.max_depth         = 50;		// 递归深度（进入场景的最大反弹次数）
    cam.background        = color(0.70, 0.80, 1.00); // 背景颜色

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

void checkered_spheres() { // 场景（含两个棋盘纹理材质的球体）
	// World
	hittable_list world; // 世界中的物体与光线相交

	// 材质、纹理
	auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9)); // 棋盘纹理（// 棋盘纹理的缩放比例，偶数纹理颜色，奇数纹理颜色）（当成材质传入物体中）

	// 物体
    world.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));	// 添加一个球体（地面）
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));	// 添加一个球体（天空）

	// Camera
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;	// 纵横比
    cam.image_width       = 400;		// 图像宽度
    cam.samples_per_pixel = 100;		// 每个像素的采样次数
    cam.max_depth         = 50;			// 递归深度（进入场景的最大反弹次数）
    cam.background        = color(0.70, 0.80, 1.00); // 背景颜色

	// 相机位置
    cam.vfov     = 20;	// 垂直视角（视野）
    cam.lookfrom = point3(13,2,3);	// 相机点(相机位置)
    cam.lookat   = point3(0,0,0);	// 观察点(相机看向的位置)
    cam.vup      = vec3(0,1,0);		// 相机的上方向(这样相机可以绕lookfrom-lookat的轴向旋转)

	// 焦平面相关（可计算光圈大小）defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2))
    cam.defocus_angle = 0;	// 圆锥体的角度，其顶点位于视口中心，底部（散焦盘）位于相机中心，可以用来换算焦平面的半径（即光圈大小），0表示无散焦

	// Render
    cam.render(world);
}

void earth() {	// 场景（地球）
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");	// 地球纹理（加载图片数据获取）
    auto earth_surface = make_shared<lambertian>(earth_texture);		// 地球表面材质（将地球纹理数据传入地球表面介质）
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);	// 地球（球体）

	// Camera
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;	// 纵横比
    cam.image_width       = 400;		// 图像宽度
    cam.samples_per_pixel = 100;		// 每个像素的采样次数
    cam.max_depth         = 50;			// 递归深度（进入场景的最大反弹次数）
    cam.background        = color(0.70, 0.80, 1.00); // 背景颜色

	// 相机位置
    cam.vfov     = 20;				// 垂直视角（视野）
    cam.lookfrom = point3(0,0,12);	// 相机点(相机位置)
    cam.lookat   = point3(0,0,0);	// 观察点(相机看向的位置)
    cam.vup      = vec3(0,1,0);		// 相机的上方向(这样相机可以绕lookfrom-lookat的轴向旋转)

    // 焦平面相关（可计算光圈大小）defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2))
    cam.defocus_angle = 0;	// 圆锥体的角度，其顶点位于视口中心，底部（散焦盘）位于相机中心，可以用来换算焦平面的半径（即光圈大小），0表示无散焦

	// Render
    cam.render(hittable_list(globe)); // 渲染地球
}

void perlin_spheres() { // 场景（柏林噪声）
    // World
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);    // 柏林噪声纹理(缩放比例4)
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext))); // 添加一个地面（地表材质为漫反射材质，纹理为柏林噪声）
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext))); // 添加一个球体（球体材质为漫反射材质，纹理为柏林噪声）

    // Camera
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0; // 纵横比
    cam.image_width       = 400;        // 图像宽度
    cam.samples_per_pixel = 100;        // 每个像素的采样次数
    cam.max_depth         = 50;         // 递归深度（进入场景的最大反弹次数）
    cam.background        = color(0.70, 0.80, 1.00); // 背景颜色

    // 相机位置
    cam.vfov     = 20;              // 垂直视角（视野）
    cam.lookfrom = point3(13,2,3);  // 相机点(相机位置)
    cam.lookat   = point3(0,0,0);   // 观察点(相机看向的位置)
    cam.vup      = vec3(0,1,0);     // 相机的上方向(这样相机可以绕lookfrom-lookat的轴向旋转)

    // 焦平面相关（可计算光圈大小）defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2))
    cam.defocus_angle = 0;  // 圆锥体的角度，其顶点位于视口中心，底部（散焦盘）位于相机中心，可以用来换算焦平面的半径（即光圈大小）

    // Render
    cam.render(world);
}

void quads() {  // 场景（四边形）
    // World
    hittable_list world;

    // 材质
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    //物体（此处为四边形）
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    // Camera
    camera cam;

    // Image
    cam.aspect_ratio      = 1.0; // 纵横比
    cam.image_width       = 400; // 图像宽度
    cam.samples_per_pixel = 100; // 每个像素的采样次数
    cam.max_depth         = 50;  // 递归深度（进入场景的最大反弹次数）
    cam.background        = color(0.70, 0.80, 1.00); // 背景颜色

    // 相机位置
    cam.vfov     = 80;              // 垂直视角（视野）
    cam.lookfrom = point3(0,0,9);   // 相机点(相机位置)
    cam.lookat   = point3(0,0,0);   // 观察点(相机看向的位置)
    cam.vup      = vec3(0,1,0);     // 相机的上方向(这样相机可以绕lookfrom-lookat的轴向旋转)

    // 焦平面相关（可计算光圈大小）defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2))
    cam.defocus_angle = 0;  // 圆锥体的角度，其顶点位于视口中心，底部（散焦盘）位于相机中心，可以用来换算焦平面的半径（即光圈大小）

    // Render
    cam.render(world);
}

void simple_light() {
    // World
    hittable_list world;

    // 添加两个球体（添加噪声纹理的漫反射材质）
    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    // 光源
    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
    world.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));

    // Camera
    camera cam;

    // Image
    cam.aspect_ratio      = 16.0 / 9.0; // 纵横比
    cam.image_width       = 400;        // 图像宽度
    cam.samples_per_pixel = 100;        // 每个像素的采样次数
    cam.max_depth         = 50;         // 递归深度（进入场景的最大反弹次数）
    cam.background        = color(0,0,0);// 背景颜色

    // 相机位置
    cam.vfov     = 20;              // 垂直视角（视野）
    cam.lookfrom = point3(26,3,6);  // 相机点(相机位置)
    cam.lookat   = point3(0,2,0);   // 观察点(相机看向的位置)
    cam.vup      = vec3(0,1,0);     // 相机的上方向(这样相机可以绕lookfrom-lookat的轴向旋转)

    // 焦平面相关（可计算光圈大小）defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2))
    cam.defocus_angle = 0;  // 圆锥体的角度，其顶点位于视口中心，底部（散焦盘）位于相机中心，可以用来换算焦平面的半径（即光圈大小）

    // Render
    cam.render(world);
}

void cornell_box() {
    // World
    hittable_list world;

    // 材质
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15)); // 漫反射光源

    // 物体
    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green)); // 左墙
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));   // 右墙
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light)); // 光源
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));   // 地面
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));   // 顶部
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white)); // 背墙

    // Camera
    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    // Render
    cam.render(world);
}

int main() {
	switch(7) {
		case 1: bouncing_spheres();     break;
        case 2: checkered_spheres();    break;
		case 3: earth();                break;
        case 4: perlin_spheres();       break;
        case 5: quads();                break;
        case 6: simple_light();         break;
        case 7: cornell_box();          break;
        // default: bouncing_spheres();    break;
	}
}
