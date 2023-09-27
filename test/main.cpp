#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unistd.h>
#include <sys/wait.h>


std::vector<std::string> read_strings() {
    std::vector<std::string> strings;
    std::string str;
    while(std::cin >> str) {
        strings.push_back(str);
    }
    return strings;
}

std::string to_lower(const std::string& str) {
    std::string res = "";
    for (char ch : str) {
        res += tolower(ch);
    }
    return res;
}

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

int main(int argc, char *argv[]) {
    
    int pipe_1_fd[2];
    if (pipe(pipe_1_fd) == -1) {
        std::cout << "An error ocurred with opening the pipe" << std::endl;
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cout << "An error ocurred with opening the process" << std::endl;
        return 2;
    }
    if (pid == 0) {
        close(pipe_1_fd[1]);
        std::cout << "You are in CHILD_1 process" << std::endl;
        std::string child_1_len;
        std::string child_1_str;
        read(pipe_1_fd[0], &child_1_len, sizeof(int));
        read(pipe_1_fd[0], &child_1_str, sizeof(char));
        std::cout << "Child str: " << child_1_str << std::endl;
        close(pipe_1_fd[0]);
    }
    if (pid > 0) {
        close(pipe_1_fd[0]);
        std::cout << "You are in PARENT process" << std::endl;
        std::cout << "Enter string in PARENT process: ";
        std::string parent_str;
        std::cin >> parent_str;
        int parent_len = parent_str.length();
        write(pipe_1_fd[1], &parent_len, sizeof(char));
        write(pipe_1_fd[1], &parent_str, sizeof(char) * parent_len);
        close(pipe_1_fd[1]);
    }

    return 0;
}