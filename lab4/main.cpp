#include <iostream>
#include <vector>

// #include "prime.h"
// #include "area.h"

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
            ++count;
        }
    }
    return count;
}

int64_t eratosthenes_prime_count(int64_t a, int64_t b) {
    std::vector<int64_t> sieve(b + 1, 1);
    sieve[0] = 0;
    sieve[1] = 0;
    for (int64_t i = 2; i <= b; ++i) {
        if (sieve[i] != 1) {
            continue;
        }
        for (int64_t j = i * 2; j <= b; j += i) {
            sieve[j] = 0;
        }
    }
    int64_t count = 0;
    for (int64_t i = a; i < b; ++i) {
        count += sieve[i];
    }
    return count;
}

double rectangle_area(double a, double b) {
    return a * b;
}

double triangle_area(double a, double b) {
    return (a * b) / 2;
}


int main() {
    std::cout << dumb_prime_count(0, 1000) << std::endl;
    std::cout << eratosthenes_prime_count(0, 1000) << std::endl;
    return 0;
}