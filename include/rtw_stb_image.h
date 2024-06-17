#pragma once

// 禁用 Microsoft Visual C++ 编译器对该头文件的严格警告。
#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION //在包含stb_image.h头文件之前定义这个宏，会让这个头文件包含stb_image库的实现部分。这是一个常见的技巧，用于将一个整个库的源代码放在一个头文件中。通常，这个宏只在你的程序中定义一次，就在你包含stb_image.h的地方。
#define STBI_FAILURE_USERMSG // 在stb_image库中，这个宏用于控制当图像加载失败时，错误消息的格式。如果定义了这个宏，stb_image库会在加载失败时，返回一个错误消息字符串，而不是直接退出程序。
#include "stb_image/stb_image.h"

#include <cstdlib>
#include <iostream>

class rtw_image {   // 读取图像文件
public:
    rtw_image() {}

    rtw_image(const char* image_filename) {
        // 从指定文件加载图像数据。如果定义了 RTW_IMAGES 环境变量 则只在该目录下查找图像文件。如果未找到图像，则首先在当前目录下查找指定的图像文件，然后在 images/ 子目录下查找，接着在_parent_的 images/ 子目录下查找，然后在_that_ parent 目录下查找，依此类推，向上查找六层。如果图片加载不成功，width() 和 height() 将返回 0。

        auto filename = std::string(image_filename);    // 图像文件名
        auto imagedir = getenv("RTW_IMAGES");           // 图像目录

        // 在一些可能的位置搜索图像文件。
        if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
        if (load(filename)) return;
        if (load("images/" + filename)) return;
        if (load("../images/" + filename)) return;
        if (load("../../images/" + filename)) return;
        if (load("../../../images/" + filename)) return;
        if (load("../../../../images/" + filename)) return;
        if (load("../../../../../images/" + filename)) return;
        if (load("../../../../../../images/" + filename)) return;

        std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";  // 输出错误信息
    }

    ~rtw_image() {  // 释放图像数据
        delete[] bdata;
        STBI_FREE(fdata);
    }

    bool load(const std::string& filename) {
        // 从给定的文件名加载线性（gamma=1）图像数据。如果加载成功，则返回 true。生成的数据缓冲区包含第一个像素的三个 [0.0, 1.0] 浮点数值（红色、绿色、蓝色）。像素是连续的，从左到右依次为图像的宽度，下一行为图像的高度。

        auto n = bytes_per_pixel; // 虚拟输出参数：每个像素的原始分量
        fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel); // 加载图像数据
        if (fdata == nullptr) return false;

        bytes_per_scanline = image_width * bytes_per_pixel; // 每行的字节数
        convert_to_bytes(); // 将浮点数据转换为字节数据
        return true;
    }

    int width()  const { return (fdata == nullptr) ? 0 : image_width; } // 返回图像宽度
    int height() const { return (fdata == nullptr) ? 0 : image_height; }// 返回图像高度

    const unsigned char* pixel_data(int x, int y) const {
        // 返回(x,y)处像素的三个 RGB 字节的地址。如果没有图像数据，则返回洋红色。
        static unsigned char magenta[] = { 255, 0, 255 };
        if (bdata == nullptr) return magenta;

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);

        return bdata + y*bytes_per_scanline + x*bytes_per_pixel;
    }

private:
    const int      bytes_per_pixel = 3;     // 每个像素的原始字节数(RGB)
    float         *fdata = nullptr;         // 线性浮点像素data
    unsigned char *bdata = nullptr;         // 线性8位像素数据
    int            image_width = 0;         // 已加载图像的宽度
    int            image_height = 0;        // 已加载图像的高度
    int            bytes_per_scanline = 0;  // 每行的字节数

    static int clamp(int x, int low, int high) { // 将x限制在[low,high)范围内
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    static unsigned char float_to_byte(float value) {   //浮点数转换为字节
        if (value <= 0.0)   return 0;
        if (1.0 <= value)   return 255;
        return static_cast< unsigned char >(256.0 * value);
    }

    void convert_to_bytes() {
        // 将线性浮点像素数据转换为字节，并将转换后的字节数据存储在 `bdata` 成员中。
        int total_bytes = image_width * image_height * bytes_per_pixel;
        bdata = new unsigned char[total_bytes];

        // 遍历所有像素分量，将 [0.0, 1.0] 浮点数值转换为 无符号 [0, 255] 字节值。
        auto *bptr = bdata;
        auto *fptr = fdata;
        for (auto i=0; i < total_bytes; i++, fptr++, bptr++)
            *bptr = float_to_byte(*fptr);
    }
};

// 恢复 MSVC 编译器警告
#ifdef _MSC_VER
    #pragma warning (pop)
#endif