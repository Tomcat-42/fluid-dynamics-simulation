#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "image.hpp"
#include "simulation.hpp"

#define SIZE 500
#define ITER 128

#define FRAMES 64

int main(const __attribute__((unused)) int argc, const __attribute__((unused)) char** argv){
    // Window window(SIZE, SIZE);

    Simulation<double, SIZE, ITER> simulation(0.1, 0.000000000000001, 0.000000000000001);

    std::cout << ":: simulation params:" << std::endl
        << " size = " << SIZE << "x" << SIZE << std::endl
        << " iter = " << ITER << std::endl
        << " dt   = " << simulation.getParam<DT>() << std::endl
        << " diff = " << simulation.getParam<DIFF>() << std::endl
        << " visc = " << simulation.getParam<VISC>() << std::endl;

    using time = std::chrono::high_resolution_clock;
    using scale = std::chrono::milliseconds;
    using timepoint = std::chrono::time_point<time, std::chrono::duration<double>>;

    auto get_time = [] -> timepoint {
        return time::now();
    };

    // NOTE: the array does include the borders
    std::uint32_t* image = new std::uint32_t[(SIZE + 2) * (SIZE + 2)];

    for (std::size_t i = 0; i < FRAMES; i++){
        auto start = get_time();

        simulation.step();

        std::cout << "\033[2K\r" << std::chrono::duration_cast<scale>(get_time() - start).count() << "(ms)" << std::flush;

        Image::genImage(image, SIZE, simulation.getD(), simulation.getU(), simulation.getV());

        std::ofstream of("images/" + std::to_string(i) + ".raw", std::ios::binary);
        of.write(reinterpret_cast<char*>(image), sizeof(std::uint8_t) * 4 * (SIZE + 2) * (SIZE + 2));
        of.close();
    }

    delete [] image;

    return 0;
}
