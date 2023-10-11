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
    read(STDIN_FILENO, &len, sizeof(int));
    std::string str(len, ' ');
    read(STDIN_FILENO, str.data(), len);
    str = delete_spaces(str);
    len = str.size();
    // std::cout << delete_spaces(str) << std::endl;
    write(STDOUT_FILENO, &len, sizeof(int));
    write(STDOUT_FILENO, str.c_str(), len);
    return 0;
}