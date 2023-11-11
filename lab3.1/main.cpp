#include <iostream>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // std::cout << "Enter your string: " << std::endl;
    // while (std::cin) {
        // Creating pipes
        
        int fd_1 = open("temp_1.txt", O_RDWR | O_CREAT | O_TRUNC , 0777);
        if (fd_1 < 0) {
            std::cout << "Error: open failed (fd1)" << std::endl;
            return 1;
        }
        struct stat st_1;

        ftruncate (fd_1 , 255);

        if (fstat(fd_1, &st_1) < 0) {
            close(fd_1);
            std::cout << "Error: fstat failed(st_1)" << std::endl;
        }
        size_t fsize_1 = (size_t)st_1.st_size;

        char* data_ptr_1 = (char*) mmap(nullptr, fsize_1, 
                                      PROT_READ | PROT_WRITE, 
                                      MAP_SHARED ,fd_1, 0);
        if (data_ptr_1 == MAP_FAILED) {
            close(fd_1);
            std::cout << "Error: mmap failed (data_ptr_1)" << std::endl;
        }
        for(int i = 0; i < 10; i++) {
            data_ptr_1[i] = 'p';
        }

        char* data_ptr_2 = (char*) mmap(nullptr, fsize_1, 
                                        PROT_READ | PROT_WRITE, 
                                        MAP_SHARED ,fd_1, 0);
        if (data_ptr_2 == MAP_FAILED) {
            close(fd_1);
            std::cout << "Error: mmap failed (data_ptr_2)" << std::endl;
        }
        

        for(int i = 10; i < 20; i++) {
            data_ptr_2[i] = 'k';
        }
        
    // } 
    //     int pid_1 = fork();

    //     if (pid_1 == 0) {
    //         // Child_1
            

    //     } else if (pid_1 > 0) {
    //         // Parent
            

    //         int pid_2 = fork();
    //         if (pid_2 == 0) {
    //             // Child_2
                

    //         } else if (pid_2 > 0) {
    //             // Parent
                
            
    //         } else {
    //             std::cout << "Error: smth wrong with pid_2";
    //             return 4;
    //         }
    //     } else {
    //         std::cout << "Error: smth wrong with pid_1";
    //         return 5;
    //     }
    // }
    // remove("temp_1.txt");
    return 0;
}