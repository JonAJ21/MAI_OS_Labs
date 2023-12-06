#include <dlfcn.h>
#include <stdlib.h>
#include "lib.h"

char const LIBS[] = {"./liblib1d.so", "./liblib2d.so"};
typedef int (*prime_count_func)(int A, int B);
typedef float (*square_func)(float A, float B);

void menu() {
    std::cout << "Enter number: " << std::endl;
    std::cout << "0. Change implementation" << std::endl;
    std::cout << "1. PrimeCount" << std::endl;
    std::cout << "2. Square" << std::endl;
}

void open_handle(void** lib_handle, int const cur_lib) {
    *lib_handle = dlopen(LIBS[cur_lib], RTLD_NOW);
    if (*lib_handle == NULL) {
        std::cout << dlerror() << std::endl;
        throw std::logic_error("Error: open_handle");
    }
}

void set_implementation(prime_count_func *PrimeCountFunc, square_func *SquareFunc, void* lib_handle) {
    *PrimeCountFunc = (int(*)(int, int))dlsym(lib_handle, "PrimeCount");
    if (*PrimeCountFunc == NULL) {
        std::cout << dlerror() << std::endl;
        throw std::logic_error("Error: set_implementation");
    }
    *SquareFunc = (float(*)(float, float))dlsym(lib_handle, "Square");
    if (*SquareFunc == NULL) {
        std::cout << dlerror() << std::endl;
        throw std::logic_error("Error: set_implementation");
    }
}

int main() {
    int n, cur_lib = 0;
    void* lib_handle;
    prime_count_func PrimeCountFunc;
    square_func SquareFunc;
    open_handle(&lib_handle, cur_lib);
    set_implementation(&PrimeCountFunc, &SquareFunc, lib_handle);
    menu();
    while (std::cin >> n) {
        if (n == 0) {
            dlclose(lib_handle);
            cur_lib = (cur_lib == 0)? 1 : 0;
            open_handle(&lib_handle, cur_lib);
            set_implementation(&PrimeCountFunc, &SquareFunc, lib_handle);
        } else if (n == 1) {
            std::cout << "Enter A, B: " << std::endl;
            int a, b;
            std::cin >> a >> b;
            std::cout << "Your PrimeCount: "<< PrimeCountFunc(a, b) << std::endl;
        } else if (n == 2) {
            std::cout << "Enter A, B: " << std::endl;
            int a, b;
            std::cin >> a >> b;
            std::cout << "Your Square: " << SquareFunc(a, b) << std::endl;
        } else {
            throw std::logic_error("Wrong value");
        }
        menu();
    }
    return 0;
}