#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    std::cout << "Push enter to start. ";
    std::string enter;
    while (getline(std::cin, enter)) {
        // Creating pipes
        int pipe_1[2];
        int pipe_add[2];
        int pipe_2[2];
        if (pipe(pipe_1) == -1) {
            std::cout << "Something wrong with pipe_1" << std::endl;
            return 1;
        }
        if (pipe(pipe_add) == -1) {
            std::cout << "Something wrong with pipe_add" << std::endl;
            return 2;
        }
        if (pipe(pipe_2) == -1) {
            std::cout << "Something wrong with pipe_2" << std::endl;
            return 3;
        }
        
        int pid_1 = fork();

        if (pid_1 == 0) {
            // Child_1
            std::cout << "Child_1 process" << std::endl;
            close(pipe_1[1]);
            close(pipe_add[0]);
            // dup2(pipe_1[0], STDIN_FILENO);
            // dup2(pipe_add[1], STDOUT_FILENO);
            if (dup2(pipe_1[0], STDIN_FILENO) == -1) {
                std::cout << "Error: dup2 1" << std::endl;
                return 20;
            }
            if (dup2(pipe_add[1], STDOUT_FILENO) == -1) {
                std::cout << "Error: dup2 2" << std::endl;
                return 21;
            }

            execl("child1", "child1", NULL);
            if (execl("child1", "child1", NULL) == -1) {
                std::cout << "Error: exec child1" << std::endl;
                return 6;
            }

        } else if (pid_1 > 0) {
            // Parent
            close(pipe_1[0]);
            std::cout << "Parent process" << std::endl;
            std::string str;
            std::cout << "Enter your string: " << std::endl;
            getline(std::cin, str, '\n');
            int len = str.length();
            if (write(pipe_1[1], &len, sizeof(int)) != sizeof(int)) {
                std::cout << "Error: write" << std::endl;
                return 8;
            }
            if (write(pipe_1[1], str.c_str(), len) != len) {
                std::cout << "Error: write" << std::endl;
                return 9;
            }
            close(pipe_1[1]);

            int pid_2 = fork();
            if (pid_2 == 0) {
                // Child_2
                close(pipe_add[1]);
                close(pipe_2[0]);
                
                // dup2(pipe_add[0], STDIN_FILENO);
                // dup2(pipe_2[1], STDOUT_FILENO);
                if (dup2(pipe_add[0], STDIN_FILENO) == -1) {
                    std::cout << "Error: dup2" << std::endl;
                    return 22;
                }
                if (dup2(pipe_2[1], STDOUT_FILENO) == -1) {
                    std::cout << "Error: dup2" << std::endl;
                    return 23;
                }
                execl("child2", "child2", NULL);
                if (execl("child2", "child2", NULL) == -1) {
                    std::cout << "Error: exec child2" << std::endl;
                    return 7;
                }

            } else if (pid_2 > 0) {
                // Parent
                int len;
                // read(pipe_2[0], &len, sizeof(int));
                if (read(pipe_2[0], &len, sizeof(int)) != sizeof(int)) {
                    std::cout << "Error: read" << std::endl;
                    return 10;
                }
                std::string str(len, ' ');
                if (read(pipe_2[0], str.data(), len) != len) {
                    std::cout << "Error: read" << std::endl;
                    return 11;
                }
                std::cout << "\nFINAL len: " << len << " str: " << str << std::endl;
                std::cout << "Push enter to continue ";
            
            } else {
                std::cout << "Error: smth wrong with pid_2";
                return 4;
            }
        } else {
            std::cout << "Error: smth wrong with pid_1";
            return 5;
        }
    }
    return 0;
}