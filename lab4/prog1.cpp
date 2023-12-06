#include "lib.h"

void menu() {
    std::cout << "Enter number of function: " << std::endl;
    std::cout << "1. PrimeCount" << std::endl;
    std::cout << "2. Square" << std::endl;
}

int main() {
    int n;
    menu();
    while(std::cin >> n) {
        if (n == 1) {
            std::cout << "Enter A, B: " << std::endl;
            int a, b;
            std::cin >> a >> b;
            std::cout << "Your PrimeCount: "<< PrimeCount(a, b) << std::endl;
        } else if (n == 2) {
            std::cout << "Enter A, B: " << std::endl;
            int a, b;
            std::cin >> a >> b;
            std::cout << "Your Square: " << Square(a, b) << std::endl;
        } else {
            throw std::logic_error("Wrong value");
        }
        menu();
    }
    return 0;
}