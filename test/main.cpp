#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    
    int x;
    if (read(STDIN_FILENO, &x, sizeof(int)) == -1) {
        std::cout << "Error" << std::endl;
    } else {
        std::cout << x << std::endl;
    }


    return 0;
}