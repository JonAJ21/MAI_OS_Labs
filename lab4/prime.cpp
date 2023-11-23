#include <iostream>
#include <vector>

#include "prime.h"

int64_t dumb_prime_count(int64_t a, int64_t b) {
    int64_t count = 0;
    for (int64_t i = a; i < b; ++i) {
        int64_t is_prime = true;
        for (int64_t j = 2; j * j <= i; ++j) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime && i > 1) {
            std::cout << i << ' ';
            ++count;
        }
    }
    std::cout << std::endl;
    return count;
}


int64_t eratosthenes_prime_count(int64_t a, int64_t b) {

    return a + b;
}