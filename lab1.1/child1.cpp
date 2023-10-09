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
    int additional_pipe[2];
    if (pipe(additional_pipe) == -1) {
        std::cout << "Error: something wrong with additional pipe" << std::endl; 
    }
    std::string str;
    std::cin >> str;
    std::cout << to_lower(str) << std::endl;
    int len = str.length();
    write(additional_pipe[1], &len, sizeof(int));
    dup2(additional_pipe[0], STDIN_FILENO);

    write(additional_pipe[1], str.c_str(), len);
    dup2(additional_pipe[0], STDIN_FILENO);

    close(additional_pipe[0]);
    close(additional_pipe[1]);

    return 0;
}