#include "lib.h"

// Dumb method
extern "C" int PrimeCount(int A, int B) {
    std::cout << "DumbPrimeCount" << std::endl;
    int count = 0;
    for (int i = A; i < B; ++i) {
        int is_prime = true;
        for (int j = 2; j * j <= i; ++j) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime && i > 1) {
            ++count;
        }
    }
    return count;
}

// Rectangle
extern "C" float Square(float A, float B) {
    std::cout << "Rectangle" << std::endl;
    return A * B;
}