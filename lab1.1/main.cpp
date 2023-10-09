#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

    // Creating pipes
    int pipe_1[2];
    // int additional_pipe[2];
    int pipe_2[2];

    if (pipe(pipe_1) == -1) {
        std::cout << "Error: something wrong with pipe_1" << std::endl;
        return 1;
    }
    // if (pipe(additional_pipe) == -1) {
    //     std::cout << "Error: something wrong with additional_pipe" << std::endl;
    //     return 2;
    // }
    if (pipe(pipe_2) == -1) {
        std::cout << "Error: something wrong with pipe_2" << std::endl;
        return 3;
    }

    int pid_1 = fork();
    if (pid_1 == 0) {
        // Child_1 process

        // Trying to read string from pipe_1
        close(pipe_1[1]);
        int len;
        // TODO: Надо как-то отловить ошибку c read
        read(pipe_1[0], &len, sizeof(int));
        dup2(pipe_1[0], STDIN_FILENO);
        close(pipe_1[0]);
        execl("child1", "child1", NULL); // Отловить ошибку
        
        

    } else if (pid_1 > 0) {
        // Parent process
        wait(NULL);
        // Trying to create Child_2 process in parent process
        int pid_2 = fork();
        if (pid_2 == 0) {
            // Child_2 process
            int len;
            std::string str;
            std::cin >> len;
            std::cin >> str;
            std::cout << "Child2: " << len << ' ' << str << std::endl;




        } else if (pid_2 > 0){
            // Parent process

            std::string str;
            int len;
            close(pipe_1[0]);
            getline(std::cin, str, '\n'); // Надо ли ловить ошибки в getline?
            // Trying to write string in pipe_1
            len = str.length();
            // TODO: Надо как-то отловить ошибку c write
            write(pipe_1[1], &len, sizeof(int));
            write(pipe_1[1], str.c_str(), len);
            close(pipe_1[1]);



        } else {
            std::cout << "Error: something wrong with pid_2" << std::endl;
            return 4;
        }
    } else {
        std::cout << "Error: something wrong with pid_1" << std::endl;
        return 5;
    }

    return 0;
}