#include <iostream>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    
    int fd_2 = open("temp_2.txt", O_RDWR);
    if (fd_2 < 0) {
        std::cout << "Error: open failed (fd2)" << std::endl;
        return 1;
    }
    int fd_3 = open("temp_3.txt", O_RDWR);
    if (fd_3 < 0) {
        std::cout << "Error: open failed (fd3)" << std::endl;
        return 2;
    }

    struct stat st_2;
    if (fstat(fd_2, &st_2) < 0) {
        close(fd_2);
        std::cout << "Error: fstat failed(st_2)" << std::endl;
        return 3;
    }

    struct stat st_3;
    if (fstat(fd_3, &st_3) < 0) {
        close(fd_3);
        std::cout << "Error: fstat failed(st_3)" << std::endl;
        return 4;
    }

    size_t fsize_2 = (size_t)st_2.st_size;
    size_t fsize_3 = (size_t)st_3.st_size;

    char* data_ptr_2 = (char*) mmap(nullptr, fsize_2, 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED ,fd_2, 0);

    char* data_ptr_3 = (char*) mmap(nullptr, fsize_3, 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED ,fd_3, 0);

    bool flag = 0;
    size_t j = 0;
    for (size_t i = 0; i < fsize_2; ++i) {
        if (data_ptr_2[i] != ' ') {
            data_ptr_3[j] = data_ptr_2[i];
            flag = 0;
            ++j;
        }
        if (data_ptr_2[i] == ' ' && flag == 0) {
            data_ptr_3[j] = data_ptr_2[i];
            flag = 1;
            ++j;
        }
    }

    close(fd_2);
    close(fd_3);

    return 0;
}