#include <iostream>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>


int const BUFFER_SIZE = 255;

int main() {
    
        int fd_1 = open("temp_1.txt", O_RDWR | O_CREAT | O_TRUNC , 0777);
        int fd_2 = open("temp_2.txt", O_RDWR | O_CREAT | O_TRUNC , 0777);
        int fd_3 = open("temp_3.txt", O_RDWR | O_CREAT | O_TRUNC , 0777);

        if (fd_1 < 0) {
            std::cout << "Error: open failed (fd1)" << std::endl;
            return 1;
        }
        if (fd_2 < 0) {
            std::cout << "Error: open failed (fd2)" << std::endl;
            return 2;
        }
        if (fd_3 < 0) {
            std::cout << "Error: open failed (fd3)" << std::endl;
            return 3;
        }

        struct stat st_1;
        struct stat st_2;
        struct stat st_3;

        ftruncate (fd_1 , BUFFER_SIZE);
        ftruncate (fd_2 , BUFFER_SIZE);
        ftruncate (fd_3 , BUFFER_SIZE);

        if (fstat(fd_1, &st_1) < 0) {
            close(fd_1);
            std::cout << "Error: fstat failed(st_1)" << std::endl;
            return 4;
        }
        if (fstat(fd_2, &st_2) < 0) {
            close(fd_2);
            std::cout << "Error: fstat failed(st_2)" << std::endl;
            return 5;
        }
        if (fstat(fd_3, &st_3) < 0) {
            close(fd_3);
            std::cout << "Error: fstat failed(st_3)" << std::endl;
            return 6;
        }

        size_t fsize_1 = (size_t)st_1.st_size;
        size_t fsize_2 = (size_t)st_2.st_size;
        size_t fsize_3 = (size_t)st_3.st_size;

        char* data_ptr_1 = (char*) mmap(nullptr, fsize_1, 
                                        PROT_READ | PROT_WRITE, 
                                        MAP_SHARED ,fd_1, 0);
        if (data_ptr_1 == MAP_FAILED) {
            close(fd_1);
            std::cout << "Error: mmap failed (data_ptr_1)" << std::endl;
            return 7;
        }

        char* data_ptr_2 = (char*) mmap(nullptr, fsize_2, 
                                        PROT_READ | PROT_WRITE, 
                                        MAP_SHARED ,fd_2, 0);
        if (data_ptr_2 == MAP_FAILED) {
            close(fd_2);
            std::cout << "Error: mmap failed (data_ptr_2)" << std::endl;
            return 8;
        }

        char* data_ptr_3 = (char*) mmap(nullptr, fsize_3, 
                                        PROT_READ | PROT_WRITE, 
                                        MAP_SHARED ,fd_3, 0);
        if (data_ptr_3 == MAP_FAILED) {
            close(fd_3);
            std::cout << "Error: mmap failed (data_ptr_3)" << std::endl;
            return 8;
        }

    

    while (std::cin) { 
        int pid_1 = fork();

        if (pid_1 == 0) {
            // Child_1
                     
            while (data_ptr_1[0] == '\0');
            if (execl("child1", "child1", NULL) == -1) {
                std::cout << "Error: exec child1" << std::endl;
                return 6;
            }


        } else if (pid_1 > 0) {
            // Parent
           
            std::string str;
            std::cout << "Enter your string: " << std::endl;
            getline(std::cin, str, '\n');
            size_t len = str.length();
            if (len > fsize_1) {
                std::cout << "Error: String is too big" << std::endl;
                return 8;
            }
            for (size_t i = 0; i < len; ++i) {
                data_ptr_1[i] = str[i];
            }
            close(fd_1);
            

            int pid_2 = fork();
            if (pid_2 == 0) {
                // Child_2
                
                while (data_ptr_2[0] == '\0');

                if (execl("child2", "child2", NULL) == -1) {
                    std::cout << "Error: exec child2" << std::endl;
                    return 6;
                }

            } else if (pid_2 > 0) {
                // Parent
                
                while(data_ptr_3[0] == '\0');

                size_t i = 0;
                while (data_ptr_3[i] != '\0' || i < fsize_3) {
                    std::cout << data_ptr_3[i];
                    ++i;
                }
                std::cout << std::endl;
                
            
            } else {
                std::cout << "Error: smth wrong with pid_2";
                return 4;
            }
        } else {
            std::cout << "Error: smth wrong with pid_1";
            return 5;
        }
    }
    remove("temp_1.txt");
    remove("temp_2.txt");
    remove("temp_3.txt");
    
    return 0;
}