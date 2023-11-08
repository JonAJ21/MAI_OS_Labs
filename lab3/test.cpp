#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
 
    int fdin = open("temp_1.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);
    struct stat sb;
    ftruncate(fdin, 10 * sizeof(int));
    if (fstat(fdin, &sb) == -1) {
        std::cout << "Error" << std::endl;
        return 1;
    }
    void* file_mem = mmap(NULL, sb.st_size, PROT_WRITE | PROT_READ , MAP_SHARED , fdin, 0);
    std::cout << sb.st_size << std::endl;

    


        



    
    remove ("temp_1.txt");

    return 0;
}