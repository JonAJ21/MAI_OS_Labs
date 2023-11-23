#include <iostream>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    
    int fd_1 = open("temp_1.txt", O_RDWR);
    if (fd_1 < 0) {
        std::cout << "Error: open failed (fd1)" << std::endl;
        return 1;
    }
    int fd_2 = open("temp_2.txt", O_RDWR);
    if (fd_2 < 0) {
        std::cout << "Error: open failed (fd2)" << std::endl;
        return 2;
    }

    struct stat st_1;
    if (fstat(fd_1, &st_1) < 0) {
        close(fd_1);
        std::cout << "Error: fstat failed(st_1)" << std::endl;
        return 3;
    }

    struct stat st_2;
    if (fstat(fd_2, &st_2) < 0) {
        close(fd_2);
        std::cout << "Error: fstat failed(st_2)" << std::endl;
        return 4;
    }

    size_t fsize_1 = (size_t)st_1.st_size;
    size_t fsize_2 = (size_t)st_2.st_size;

    char* data_ptr_1 = (char*) mmap(nullptr, fsize_1, 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED ,fd_1, 0);

    char* data_ptr_2 = (char*) mmap(nullptr, fsize_2, 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED ,fd_2, 0);

    for (size_t i = 0; i < fsize_1; ++i) {
        data_ptr_2[i] = tolower(data_ptr_1[i]);
    }

    close(fd_1);
    close(fd_2);
    return 0;
}