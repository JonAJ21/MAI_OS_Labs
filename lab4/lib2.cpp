#include <vector>
#include "lib.h"

// Eratospehenes sieve
extern "C" int PrimeCount(int A, int B) {
    std::cout << "Eratospehenes sieve" << std::endl;
    std::vector<int> sieve(B + 1, 1);
    sieve[0] = 0;
    sieve[1] = 0;
    for (int i = 2; i <= B; ++i) {
        if (sieve[i] != 1) {
            continue;
        }
        for (int j = i * 2; j <= B; j += i) {
            sieve[j] = 0;
        }
    }
    int count = 0;
    for (int i = A; i < B; ++i) {
        count += sieve[i];
    }
    return count;
}

// Triangle
extern "C" float Square(float A, float B) {
    std::cout << "Triangle" << std::endl;
    return (A * B) / 2;
}