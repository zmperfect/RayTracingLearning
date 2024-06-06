#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"


color ray_color(const ray& r, const hittable& world) {
	hit_record rec; // 记录射线与物体的交点信息
	if (world.hit(r, interval(0, infinity), rec)) { // 如果射线与某个物体相交，则返回该交点的颜色
		return 0.5 * (rec.normal + color(1,1,1));
	}

    vec3 unit_direction = unit_vector(r.direction());// 获取射线的方向，并将其转换为单位向量
    auto a = 0.5*(unit_direction.y() + 1.0);// 计算一个因子a，它的值在0到1之间。当射线的方向向上时（y分量为1），a的值为1；当射线的方向向下时（y分量为-1），a的值为0。
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);// 返回一个颜色，它是白色（1.0, 1.0, 1.0）和蓝色（0.5, 0.7, 1.0）的线性插值。当a为0时，返回白色；当a为1时，返回蓝色。当a在0和1之间时，返回两种颜色的混合。
}

int main() {

	// Image
	
	auto aspect_ratio = 16.0 / 9.0; // 纵横比
	int image_width = 400; // 图像宽度

	// 计算图像高度且其至少为1
	int image_height = static_cast<int>(image_width / aspect_ratio);
	image_height = (image_height < 1) ? 1 : image_height;

	// World

	hittable_list world; // 世界中的物体与光线相交

	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5)); // 添加一个球体
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // 添加一个半径为100的球体

	// Camera

	auto focal_length = 1.0; // 焦距
	auto viewport_height = 2.0; // 视口高度
	auto viewport_width = viewport_height * (double(image_width)/image_height); // 视口宽度
	auto camera_center = point3(0,0,0); // 相机中心

	// 计算横向和纵向视口边缘的矢量V_u,V_v
	auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // 计算像素间的水平和垂直三角矢量(本质是两个方向上像素的间隔)
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // 计算左上角像素的位置(边界像素的位置到边缘的距离仅为像素间隔的一半)
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	// Render
	
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < image_height; j++) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;// 输出剩余扫描线
		for (int i = 0; i < image_width; ++i) {
			auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v); // 计算像素中心位置
            auto ray_direction = pixel_center - camera_center; // 计算射线方向
            ray r(camera_center, ray_direction); // 创建射线

            color pixel_color = ray_color(r, world); // 计算射线与world中可接触物体的像素颜色
            write_color(std::cout, pixel_color); // 写入像素颜色
		}  
	}
	std::clog << "\rDone.                 \n";// 输出完成
}
