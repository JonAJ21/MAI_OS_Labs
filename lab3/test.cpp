#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


struct FileMapping {
    int fd;
    size_t fsize;
    char* data_ptr;
    std::string data_str;
    FileMapping(std::string file_name, std::string data) {
        int fd = open(file_name.data(), O_RDWR | O_CREAT | O_TRUNC , 0777);
        write(fd, data.data(), data.size());
        // int fd = open(file_name.data(), O_RDONLY , 0777);
        if (fd < 0) {
            std::cout << "Error: open failed" << std::endl;
            throw "Error: open failed"; // Так делать плохо
        }
        struct stat st;
        if (fstat(fd, &st) < 0) {
            close(fd);
            std::cout << "Error: fstat failed" << std::endl;
            throw "Error: fstat failed"; // Так делать плохо
        }
        fsize = (size_t)st.st_size;
        data_ptr = (char*) mmap(nullptr, fsize, 
                                PROT_READ, 
                                MAP_SHARED ,fd, 0);                 
        if (data_ptr == MAP_FAILED) {
            close(fd);
            std::cout << "Error: mmap failed" << std::endl;
            throw "Error: mmap failed"; // Так делать плохо
        }
        data_str = std::string(data_ptr);
    }
    ~FileMapping() {
        munmap(data_ptr, fsize);
        close(fd);
    }
};





int main() {
    
    FileMapping map_1("temp_1.txt", "JDkld;sl;'dsl;'dsl;'lds");
    std::cout << map_1.data_str << std::endl;



    return 0;
}