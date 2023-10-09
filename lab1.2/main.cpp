#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    // std::cout << "How many strings do you want to enter: ";
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
            dup2(pipe_1[0], STDIN_FILENO);
            execl("child1", "child1", NULL);

        } else if (pid_1 > 0) {
            // Parent

            close(pipe_1[0]);
            std::cout << "Parent process" << std::endl;
            std::string str;
            std::cout << "Enter your string: " << std::endl;
            getline(std::cin, str, '\n');
            int len = str.length();
            // write(pipe_1[1], &len, sizeof(int));
            write(pipe_1[1], str.c_str(), len);
            close(pipe_1[1]);
            wait(0);


            int pid_2 = fork();
            if (pid_2 == 0) {
                // Child_2
                
                std::cout << "Child_2 process" << std::endl;
                int len;
                std::cin >> len;
                // fscanf(stdin, "%d", &len);
                std::cout << "len: " << len << std::endl;


                // dup2(STDOUT_FILENO, pipe_add[1]);
                // std::cout << "Child_2 process" << std::endl;
                // close(pipe_add[1]);
                // int len;
                // std::string str;
                // std::cout << "Child_2 process middle" << std::endl;
                // read(pipe_add[0], &len, sizeof(int));
                // close(pipe_add[0]);
                // std::cout << "len: " << len << std::endl;
                // std::cout << "Child_2 process end" << std::endl;


                

                exit(2);
            } else if (pid_2 > 0) {
                // Parent
                
                wait(0);
                std::cout << "Push enter to continue. ";
            
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