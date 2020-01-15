#include <iostream>
#include "src/dimg.h"

int main(int argc, char const *argv[])
{
    dimg api;
    api.init();
    //api.dimg_negative("./zelda.jpg", "./output_negative_GPU.jpg", DIMG_HARDWARE_ACCELERATION);
    //api.dimg_negative("./zelda.jpg","./output_negative_CPU.jpg",DIMG_CPU_ONLY);
    //api.dimg_grayscale("./zelda.jpg", "./output_grayscale_GPU.jpg", DIMG_HARDWARE_ACCELERATION);
    //api.dimg_grayscale("./zelda.jpg", "./output_grayscale_CPU.jpg", DIMG_CPU_ONLY);
    //api.dimg_black_and_white("./zelda.jpg", "./output_blackandwhite_GPU.jpg",127, DIMG_HARDWARE_ACCELERATION);
    //api.dimg_black_and_white("./zelda.jpg", "./output_blackandwhite_CPU.jpg", 127, DIMG_CPU_ONLY);
    //api.dimg_mean_blur("./zelda.jpg", "./output_mean_GPU.jpg", 7, 7, DIMG_HARDWARE_ACCELERATION);
    //api.dimg_mean_blur("./zelda.jpg", "./output_mean_CPU.jpg", 7, 7, DIMG_CPU_ONLY);
    //api.dimg_median("./zelda.jpg", "./output_median_GPU.jpg", 3, 7, DIMG_HARDWARE_ACCELERATION);
    //api.dimg_median("./zelda.jpg", "./output_median_CPU.jpg", 3, 3, DIMG_CPU_ONLY);
    //api.dimg_sobel_edge_detection("./zelda.jpg", "./output_sobel_GPU.jpg", 3, 3, DIMG_HARDWARE_ACCELERATION);
    //api.dimg_sobel_edge_detection("./zelda.jpg", "./output_sobel_CPU.jpg", 3, 3, DIMG_CPU_ONLY);
    //api.dimg_roberts_edge_detection("./zelda.jpg", "./output_roberts_GPU.jpg", 4, 4, DIMG_HARDWARE_ACCELERATION);
    //api.dimg_roberts_edge_detection("./zelda.jpg", "./output_roberts_CPU.jpg", 4, 4, DIMG_CPU_ONLY);
    //api.dimg_prewitt_edge_detection("./zelda.jpg", "./output_prewitt_GPU.jpg", 6, 6, DIMG_HARDWARE_ACCELERATION);
    //api.dimg_prewitt_edge_detection("./zelda.jpg", "./output_prewitt_CPU.jpg", 6, 6, DIMG_CPU_ONLY);
    //api.dimg_log_edge_detection("./zelda.jpg", "./output_log_GPU.jpg", 7, 7, 1.1f, DIMG_HARDWARE_ACCELERATION);
    //api.dimg_log_edge_detection("./zelda.jpg", "./output_log_CPU.jpg", 7, 7, 1.1f, DIMG_CPU_ONLY);
    //api.dimg_toon_shading("./zelda.jpg", "./output_toon_shading_GPU.jpg", 5, 5, DIMG_HARDWARE_ACCELERATION);
    api.dimg_histogram("./zelda.jpg", "./histogram_blue.jpg", 256, 150, 2);
    api.dimg_terminate();
    return 0;
}
