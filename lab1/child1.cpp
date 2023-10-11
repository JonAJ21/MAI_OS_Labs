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
    read(STDIN_FILENO, &len, sizeof(int));
    std::string str(len, ' ');
    read(STDIN_FILENO, str.data(), len);
    str = to_lower(str);
    write(STDOUT_FILENO, &len, sizeof(int));
    write(STDOUT_FILENO, str.c_str(), len);
    return 0;
}