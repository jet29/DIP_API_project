#include <iostream>
#include <stdio.h>
#include <chrono>
#include "src/dimg.h"

using namespace std;
using namespace std::chrono;

void printBenchmark(dimg api) {

    //NEGATIVE FILTER COMPARISON

    auto startCPU = high_resolution_clock::now();
    api.dimg_negative("./zelda.jpg", "./output_negative_CPU.jpg", DIMG_CPU_ONLY);
    auto stopCPU = high_resolution_clock::now();
    auto durationCPU = duration_cast<seconds>(stopCPU - startCPU);

    auto startGPU = high_resolution_clock::now();
    api.dimg_negative("./zelda.jpg", "./output_negative_GPU.jpg", DIMG_HARDWARE_ACCELERATION);
    auto stopGPU = high_resolution_clock::now();
    auto durationGPU = duration_cast<seconds>(stopGPU - startGPU);

    cout << "Negative Filter comparison: (seconds)" << endl;
    cout <<"CPU: "<<durationCPU.count() <<" VS "<<" GPU: "<<durationGPU.count()<< endl;

    //GRAYSCALE FILTER COMPARISON

    startCPU = high_resolution_clock::now();
    api.dimg_grayscale("./zelda.jpg", "./output_grayscale_CPU.jpg", DIMG_CPU_ONLY);
    stopCPU = high_resolution_clock::now();
    durationCPU = duration_cast<seconds>(stopCPU - startCPU);

    startGPU = high_resolution_clock::now();
    api.dimg_grayscale("./zelda.jpg", "./output_grayscale_GPU.jpg", DIMG_HARDWARE_ACCELERATION);
    stopGPU = high_resolution_clock::now();
    durationGPU = duration_cast<seconds>(stopGPU - startGPU);

    cout << "Grayscale Filter comparison: (seconds)" << endl;
    cout << "CPU: " << durationCPU.count() << " VS " << " GPU: " << durationGPU.count() << endl;

    //MEAN FILTER COMPARISON

    startCPU = high_resolution_clock::now();
    api.dimg_mean_blur("./zelda.jpg", "./output_mean_CPU.jpg", 7, 7, DIMG_CPU_ONLY);
    stopCPU = high_resolution_clock::now();
    durationCPU = duration_cast<seconds>(stopCPU - startCPU);

    startGPU = high_resolution_clock::now();
    api.dimg_mean_blur("./zelda.jpg", "./output_mean_GPU.jpg", 7, 7, DIMG_HARDWARE_ACCELERATION);
    stopGPU = high_resolution_clock::now();
    durationGPU = duration_cast<seconds>(stopGPU - startGPU);

    cout << "Mean Filter comparison: (seconds)" << endl;
    cout << "CPU: " << durationCPU.count() << " VS " << " GPU: " << durationGPU.count() << endl;

    //TOON SHADING COMPARISON

    startCPU = high_resolution_clock::now();
    api.dimg_toon_shading("./zelda.jpg", "./output_toon_shading_CPU.jpg", 5, 5, DIMG_CPU_ONLY);
    stopCPU = high_resolution_clock::now();
    durationCPU = duration_cast<seconds>(stopCPU - startCPU);

    startGPU = high_resolution_clock::now();
    api.dimg_toon_shading("./zelda.jpg", "./output_toon_shading_GPU.jpg", 5, 5, DIMG_HARDWARE_ACCELERATION);
    stopGPU = high_resolution_clock::now();
    durationGPU = duration_cast<seconds>(stopGPU - startGPU);

    cout << "Toon shading Filter comparison: (seconds)" << endl;
    cout << "CPU: " << durationCPU.count() << " VS " << " GPU: " << durationGPU.count() << endl;



    getchar();
}

int main(int argc, char const *argv[])
{
    dimg api;
    api.init();
    
    api.dimg_negative("./zelda.jpg", "./output_negative_GPU.jpg", DIMG_HARDWARE_ACCELERATION);
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
    //api.dimg_histogram("./zelda.jpg", "./histogram_blue.jpg", 800, 600, DIMG_GREEN);
    //int unique_colors;
    //api.dimg_unique_colors("./zelda.jpg", unique_colors);
    //std::cout<<"unique colors: "<< unique_colors <<std::endl;
    //int width, height;
    //api.dimg_image_dimension("./zelda.jpg",width, height);
    //printf("dimensions(%i,%i)\n", width, height);
    //int bpp;
    //api.dimg_image_bpp("./zelda.jpg", bpp);
    //std::cout<<"bpp: "<< bpp <<std::endl;
    //int dpi;
    //api.dimg_image_dpi("./zelda.jpg", dpi);
    //std::cout << "dpi: " << dpi << std::endl;
    
    /*float kernel[] = { 1, 1,
                      -1,-1 };
    api.dimg_custom_filter("./zelda.jpg", "./output_custom_GPU.jpg", 2, 2, kernel, DIMG_HARDWARE_ACCELERATION);
    api.dimg_custom_filter("./zelda.jpg", "./output_custom_CPU.jpg", 2, 2, kernel, DIMG_CPU_ONLY);*/


    //printBenchmark(api);

    api.dimg_terminate();

    return 0;
}
