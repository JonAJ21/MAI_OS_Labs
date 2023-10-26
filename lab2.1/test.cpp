#include <iostream>
#include <vector>
#include <pthread.h>


struct myArgs {
    int id;
    std::vector<int> v;
};

void* foo(void* args) {
    myArgs *arg = (myArgs*) args;
    int i = arg->id;
    if (i == 0) {
        for (int j = 0; j < 3; j++){
            arg->v[j] = 10 + j;
        }
    }
    if (i == 1) {
        for (int j = 0; j < 3; j++){
            arg->v[j + 3] = 20 + j;
        }
    } 
    return NULL;
}


int main() {
    
    pthread_t threads[2];
    int i;
    // int status_addr;
    myArgs my_args[2];
    for (i = 0; i < 2; ++i) {
        my_args[i] = {i, {1,2,3,4,5,6}};
    }
    for (i = 0; i < 2; ++i) {
        if (pthread_create(&threads[i], NULL, foo, (void*) &my_args[i]) != 0) {
            std::cout << "ERROR create" << std::endl;
        }
        std::cout << "Created" << i << std::endl;
    }

    for (i = 0; i < 2; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            std::cout << "ERROR join" << '{' << i << '}' << std::endl;
            return 3;
        }
        std::cout << "Joined" << i << std::endl;
    }
    for (i = 0; i < 6; ++i) {
        std::cout << my_args[0].v[i] << ' ';
    }
    std::cout << std::endl;
    for (i = 0; i < 6; ++i) {
        std::cout << my_args[1].v[i] << ' ';
    }

    return 0;
}