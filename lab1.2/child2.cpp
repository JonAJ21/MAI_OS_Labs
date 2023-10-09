#include <iostream>
#include <string>

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

int main(int argc, char* argv[]) {
    std::string str;
    std::cin >> str;
    std::cout << delete_spaces(str) << std::endl;
    return 0;
}