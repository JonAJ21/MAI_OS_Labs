// Вопросы:
// Почему выводит все по порядку, а не в перемешку? или так и должно быть?
// Как бороться read и write, если всё будет в перемешку?
// Правильно ли описал схему из варианта лабы?
// Как представить child и parent в разных программах?

#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <sys/wait.h>

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

int main(int argc, char* argv[]) {

    int pfd1[2];
    int addpfd[2];
    int pfd2[2];
    if (pipe(pfd1) == -1) {
        std::cout << "Error: pfd1 does not work" << std::endl;
        return 1;
    }
    if (pipe(addpfd) == -1) {
        std::cout << "Error: addpfd does not work" << std::endl;
        return 2;
    }
    if (pipe(pfd2) == - 1) {
        std::cout << "Error: pfd2 does not work" << std::endl;
    }

    int pid1 = fork();

    if (pid1 > 0) {
        // Parent process
        close(pfd1[0]);

        // Trying to read strings from stdin
        std::vector<std::string> strings;
        std::string str;
        // while (std::cin >> str) {
        //     strings.push_back(str);
        // }
        while (getline(std::cin, str, '\n')) {
            strings.push_back(str);
        }


        // Trying to write vector_len to pipe1
        int vector_len = strings.size();
        write(pfd1[1], &vector_len, sizeof(int));
        std::cout << "vector_len in parent: " << vector_len << std::endl;

        // Trying to write strings to pipe1
        for (auto string : strings) {
            int string_len = string.length();
            write(pfd1[1], &string_len, sizeof(int));
            std::cout << "string_len in parent: " << string_len << std::endl;
            
            write(pfd1[1], string.c_str(), string_len);
            std::cout << "string in parent: " << string << std::endl;

        }

        close(pfd1[1]);

        // Trying to make child_2
        int pid2 = fork();
        if (pid2 > 0) {
            // Still parent process
            close(pfd2[1]);
            
            // Trying to read strings from pipe2
            std::vector<std::string> strings;
            // Trying to read vector_len from pipe2
            int vector_len;
            read(pfd2[0], &vector_len, sizeof(int));
            std::cout << "vector_len in parent: " << vector_len << std::endl;
            
            // Trying to read strings from pipe2
            for (int i = 0; i < vector_len; ++i) {
                int string_len;
                read(pfd2[0], &string_len, sizeof(int));
                std::cout << "string_len in parent: " << string_len << std::endl;

                std::string string(string_len, ' ');

                read(pfd2[0], string.data(), string_len);
                std::cout << "string in parent: " << string << std::endl;
                strings.push_back(string);
            }
            close(pfd2[0]);
            std::cout << "===================" << std::endl;
            for (auto el : strings) {
                std::cout << "Final: vector_string from parent: " << el << std::endl;
            }
            std::cout << "===================" << std::endl;


            

        } else if (pid2 == 0) {
            // Child2 process
            close(addpfd[1]);
            close(pfd2[0]);
            // Trying to read strings from additional pipe
            std::vector<std::string> strings;
            // Trying to read vector_len from additional pipe
            int vector_len;
            read(addpfd[0], &vector_len, sizeof(int));
            std::cout << "vector_len in child_2: " << vector_len << std::endl;
            
            // Trying to read strings from additional pipe
            for (int i = 0; i < vector_len; ++i) {
                int string_len;
                read(addpfd[0], &string_len, sizeof(int));
                std::cout << "string_len in child_2: " << string_len << std::endl;

                std::string string(string_len, ' ');

                read(addpfd[0], string.data(), string_len);
                std::cout << "string in child_2: " << string << std::endl;
                strings.push_back(string);
            }
            close(addpfd[0]);

            std::cout << "===================" << std::endl;

            // Print strings before delete_spaces
            for (auto el : strings) {
                std::cout << "vector_string from child_2: " << el << std::endl;
            }

            std::cout << "===================" << std::endl;

            // Trying delete_spaces
            for (int i = 0; i < vector_len; ++i) {
                strings[i] = delete_spaces(strings[i]);
            }

            // Print strings after delete_spaces
            for (auto el : strings) {
                std::cout << "vector_string from child_2: " << el << std::endl;
            }
            std::cout << "===================" << std::endl;


            //----------------------------------
            // Trying to write strings to pfd2
            
            // Trying to write vector_len to pipe2
            write(pfd2[1], &vector_len, sizeof(int));
            std::cout << "vector_len in child2: " << vector_len << std::endl;
            
            // Trying to write strings to pipe2
            for (auto string : strings) {
                int string_len = string.length();
                write(pfd2[1], &string_len, sizeof(int));
                std::cout << "string_len in child2: " << string_len << std::endl;
                
                write(pfd2[1], string.c_str(), string_len);
                std::cout << "string in child2: " << string << std::endl;

            }
            close(pfd2[1]);




        }




    } else if (pid1 == 0) {
        // Child1 process
        close(pfd1[1]);
        close(addpfd[0]);
        // Trying to read strings from pipe1
        std::vector<std::string> strings;
        // Trying to read vector_len from pipe1
        int vector_len;
        read(pfd1[0], &vector_len, sizeof(int));
        std::cout << "vector_len in child_1: " << vector_len << std::endl;
        
        // Trying to read strings from pipe1
        for (int i = 0; i < vector_len; ++i) {
            int string_len;
            read(pfd1[0], &string_len, sizeof(int));
            std::cout << "string_len in child_1: " << string_len << std::endl;

            std::string string(string_len, ' ');

            read(pfd1[0], string.data(), string_len);
            std::cout << "string in child_1: " << string << std::endl;
            strings.push_back(string);
        }
        
        close(pfd1[0]);

        std::cout << "===================" << std::endl;

        // Print strings before to_lower
        for (auto el : strings) {
            std::cout << "vector_string from child_1: " << el << std::endl;
        }

        std::cout << "===================" << std::endl;

        // Trying to_lower
        for (int i = 0; i < vector_len; ++i) {
            strings[i] = to_lower(strings[i]);
        }

        // Print strings after to_lower
        for (auto el : strings) {
            std::cout << "vector_string from child_1: " << el << std::endl;
        }
        std::cout << "===================" << std::endl;

        // Trying to write vector_len to additional pipe
        write(addpfd[1], &vector_len, sizeof(int));
        std::cout << "vector_len in child1: " << vector_len << std::endl;
        
        // Trying to write strings to additional pipe
        for (auto string : strings) {
            int string_len = string.length();
            write(addpfd[1], &string_len, sizeof(int));
            std::cout << "string_len in child1: " << string_len << std::endl;
            
            write(addpfd[1], string.c_str(), string_len);
            std::cout << "string in child1: " << string << std::endl;

        }
        close(addpfd[1]);
    }



    return 0;
}