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


int main(int argc, char* argv[]) {
    int p[2];
    if (pipe(p) == -1) {
        std::cout << "Error: pipe in child_1";
        return 10;
    }

    std::string str;
    std::cin >> str;
    int len = str.size();
    std::cout << len << ' ' << to_lower(str) << std::endl;

    write(p[1], &len, sizeof(int));
    close(p[1]);
    read(STDIN_FILENO, &len, sizeof(int));
    int k;
    std::cin >> k;

    // close(p[0]);
    std::cout << "Child1.cpp k: " << k << std::endl;
    return 0;
}