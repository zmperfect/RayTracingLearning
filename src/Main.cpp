#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

int main() {
	// World
	hittable_list world; // 世界中的物体与光线相交

	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5)); // 添加一个球体
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // 添加一个半径为100的球体

	// Camera
	camera cam; // 相机

	cam.aspect_ratio 		= 16.0 / 9.0; // 设置纵横比
	cam.image_width  		= 400; // 设置图像宽度
	cam.samples_per_pixel 	= 100; // 设置每个像素的采样次数
	cam.max_depth         	= 50; // 设置递归深度

	// Render
    cam.render(world);
}
