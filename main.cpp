#include <iostream>
#include "src/dimg.h"

int main(int argc, char const *argv[])
{
    dimg api;
    api.init();
    api.dimg_negative("./zelda.jpg","./output_negative.jpg",1);
    api.dimg_grayscale("./zelda.jpg", "./output_grayscale.jpg", 1);
    api.dimg_black_and_white("./zelda.jpg", "./output_blackandwhite.jpg",127, 1);
    std::cout << "=====================================================" << std::endl
              << "        Press Escape to close the program            " << std::endl
              << "=====================================================" << std::endl;

    return 0;
}
