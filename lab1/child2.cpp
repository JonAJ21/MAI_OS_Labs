#include <iostream>
#include <string>

#include <unistd.h>

std::string delete_spaces(const std::string& str) {
    std::string res = "";
    bool flag = 0;
    for (char ch : str) {
        if (ch != ' ') {
            res += ch;
            flag = 0;
        }
        if (ch == ' ' and flag == 0) {
            res += ch;
            flag = 1;
        }
    }
    return res;
}

int main() {
    int len;
    if (read(STDIN_FILENO, &len, sizeof(int)) != sizeof(int)) {
        std::cout << "Error: read" << std::endl;
        return 16;
    }
    std::string str(len, ' ');
    if (read(STDIN_FILENO, str.data(), len) != len) {
        std::cout << "Error: read" << std::endl;
        return 17;
    }
    str = delete_spaces(str);
    len = str.size();
    if (write(STDOUT_FILENO, &len, sizeof(int)) != sizeof(int)) {
        std::cout << "Error: write" << std::endl;
        return 18;
    }
    if (write(STDOUT_FILENO, str.c_str(), len) != len) {
        std::cout << "Error: write" << std::endl;
        return 19;
    }
    return 0;
}