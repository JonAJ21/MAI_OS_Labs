#include <iostream>
#include <string>

#include <unistd.h>

std::string to_lower(const std::string& str) {
    std::string res = "";
    for (char ch : str) {
        res += tolower(ch);
    }
    return res;
}


int main() {
    int len;
    if (read(STDIN_FILENO, &len, sizeof(int)) != sizeof(int)) {
        std::cout << "Error: read" << std::endl;
        return 12;
    }
    std::string str(len, ' ');
    if (read(STDIN_FILENO, str.data(), len) != len) {
        std::cout << "Error: read" << std::endl;
        return 13;
    }
    str = to_lower(str);
    if (write(STDOUT_FILENO, &len, sizeof(int)) != sizeof(int)) {
        std::cout << "Error: write" << std::endl;
        return 14;
    }
    if (write(STDOUT_FILENO, str.c_str(), len) != len) {
        std::cout << "Error: write" << std::endl;
        return 15;
    }
    return 0;
}