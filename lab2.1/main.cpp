#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include <pthread.h>
#include <unistd.h>

int const MAX_COORD = 20; // Задаю квадрат 20x20
int const POINTS_COUNT = 30; // Задаю количество точек
int const STEPS = 30; // Количество итераций, за которое мы будем искать кластеры
int const MAX_THREADS = 4;
pthread_mutex_t mutex;


struct Point {
    int x;
    int y;
    int claster; // == -1 : не привязана, == i: привязана к i кластеру
};

struct Claster {
    Point center;
    std::vector<Point> points;
};

struct myArgs {
    int thread_id;
    std::vector<Point> points;
    std::vector<Claster> clasters;
};

// Делаю случайную выбоку точек
std::vector<Point> make_random_sample(int count, int max_num);
// Дистанция от точки до точки
double distance(Point lhs, Point rhs);

// «Привязываем» каждую точку к тому центроиду, к которому она ближе
void* connect_point(void* args);
// Ищем новый центр масс кластера
void* find_center(void* args);

int main() {
    int k; // Число кластеров
    std::cout << "Введите количество кластеров" << std::endl;
    std::cin >> k;

    std::srand(std::time(nullptr));
    std::vector<Point> points = make_random_sample(POINTS_COUNT, MAX_COORD);
    std::vector<Claster> clasters(k);

    myArgs args = {-1, points, clasters};

    // Сначала случайным образом выставляем на этой же плоскости k центров кластеров
    for (int i = 0; i < k; ++i) {
        args.clasters[i].center.x = std::rand() % MAX_COORD;
        args.clasters[i].center.y = std::rand() % MAX_COORD;
        args.clasters[i].center.claster = i;
    }

    // Записываем начальные координаты в файл (для дальнейшей визуализации)
    std::ofstream out;
    out.open("out.txt");
    if (out.is_open()) {
        // Записываем в файл
        out << MAX_COORD << ' ' << POINTS_COUNT << std::endl;
        out << k << std::endl;
    }
    
    pthread_t threads[MAX_THREADS];
    // Выделить массив аргументов
    std::vector<myArgs>
    // Избавиться от мьютексов
    // Создать массив моих аргументов (i-й thread , указатель на args)

    // Запускаем цикл поиска кластеров
    for (int step = 0; step < STEPS; ++step) {
        // Очищаем точки принадлежащие кластеру
        for (int i = 0; i < k; ++i) {
            args.clasters[i].points.clear();
        }
        // Сначала мы «привязываем» каждую точку к тому центроиду, к которому она ближе
        // Надо делать параллельно
        pthread_mutex_init(&mutex, NULL);
        int idx;
        for (idx = 0; idx < MAX_THREADS; ++idx) {
            pthread_mutex_lock(&mutex);
            args.thread_id = idx;
            if (pthread_create(&threads[idx], NULL, connect_point, (void*) &args) != 0) {
                std::cout << "Error: thread did not create " << idx << std::endl;
                return 1;
            }
            // std::cout << "Created " << idx << std::endl;
        }
        for (idx = 0; idx < MAX_THREADS; ++idx) {
            if (pthread_join(threads[idx], NULL) != 0) {
                std::cout << "Error: thread did not join " << idx << std::endl;
                return 2;
            }
            // std::cout << "Joined " << idx << std::endl;
        }
        pthread_mutex_destroy(&mutex);
        
        // Передаем начальные кластеры в файл
        // Параллельно делать не буду т.к может неправильно записаться в файл,
        // можно сделать MUTEX, но есть ли в этом смысл
        if (step == 0) {
            for (int i = 0; i < k; ++i) {
                out << i << ' ' << args.clasters[i].points.size() << ' ' <<
                args.clasters[i].center.x << ' ' << args.clasters[i].center.y << std::endl;
                for (auto point : args.clasters[i].points) {
                    out << point.x << ' ' << point.y << std::endl;
                }
            }
        }
        

        // Ищем новый центр масс кластера
        // Надо делать параллельно
        for (int i = 0; i < k; ++i) {
            args.clasters[i].center = {0, 0, i};
        }
        pthread_mutex_init(&mutex, NULL);
        for (idx = 0; idx < MAX_THREADS; ++idx) {
            pthread_mutex_lock(&mutex);
            args.thread_id = idx;
            if (pthread_create(&threads[idx], NULL, find_center, (void*) &args) != 0) {
                std::cout << "Error: thread did not create " << idx << std::endl;
                return 3;
            }
            // std::cout << "Created " << idx << std::endl;
        }
        for (idx = 0; idx < MAX_THREADS; ++idx) {
            if (pthread_join(threads[idx], NULL) != 0) {
                std::cout << "Error: thread did not join " << idx << std::endl;
                return 4;
            }
            // std::cout << "Joined " << idx << std::endl;
        }
        pthread_mutex_destroy(&mutex);
        
        // Проверка выделили мы все кластеры (Если нет, то рандомим все центры еще раз)
        int flag = 0;
        for (int i = 0; i < k; ++i) {
            if (args.clasters[i].points.size() == 0) {
                for (int j = 0; j < k; ++j) {
                    args.clasters[j].center.x = std::rand() % MAX_COORD;
                    args.clasters[j].center.y = std::rand() % MAX_COORD;
                }
                flag = 1;
                std::cout << "ZEERROORORO" << std::endl;
                break;
            }
        }
        if (flag == 1) {
            continue;
        }


        for (int i = 0; i < k; ++i) {
            args.clasters[i].center.x /= args.clasters[i].points.size();
            args.clasters[i].center.y /= args.clasters[i].points.size();
        }
    }

    // Передаем финальные кластеры в файл
    for (int i = 0; i < k; ++i) {
        out << i << ' ' << args.clasters[i].points.size() << ' ' <<
        args.clasters[i].center.x << ' ' << args.clasters[i].center.y << std::endl;
        for (auto point : args.clasters[i].points) {
            out << point.x << ' ' << point.y << std::endl;
        }
    }
 

    out.close();
    return 0;
}

void* find_center(void* args) {
    myArgs *arg = (myArgs*) args;
    int thread_id = arg->thread_id;
    pthread_mutex_unlock(&mutex);
    
    // std::cout << "DEBUG(center): " <<  lhs << ' ' << rhs << ' ' << thread_id << std::endl;
    for (int i = 0; i < (int)arg->clasters.size(); ++i) {
        int points_cnt = (int)arg->clasters[i].points.size();
        for (int j = thread_id; j < points_cnt; j += MAX_THREADS) {
            arg->clasters[i].center.x += arg->clasters[i].points[j].x;
            arg->clasters[i].center.y += arg->clasters[i].points[j].y;
        }

        // TODO проверка выделили мы все кластеры (Если нет, то рандомим все центры еще раз)
    
    }
    
    return NULL;
}



void* connect_point(void* args) {
    myArgs *arg = (myArgs*) args;
    int thread_id = arg->thread_id;
    pthread_mutex_unlock(&mutex);
    int points_cnt = (int)arg->points.size();
    // std::cout << "DEBUG(connect): " <<  lhs << ' ' << rhs << ' ' << thread_id << std::endl;
    for (int j = thread_id; j < points_cnt; j += MAX_THREADS) {
        int closest_claster_id = 0;
        double closest_distance = 1e9;
        for (int i = 0; i < (int)arg->clasters.size(); ++i) {
            if (distance(arg->clasters[i].center, arg->points[j]) < closest_distance) {
                closest_distance = distance(arg->clasters[i].center, arg->points[j]);
                closest_claster_id = i;
            }
        }
        arg->points[j].claster = closest_claster_id;
        arg->clasters[closest_claster_id].points.push_back(arg->points[j]);
    }
    //DEBUG
    // for (int i = 0; i < (int)arg->clasters.size(); ++i) {
    //     std::cout << "Claster " << i << ' ' << arg->clasters[i].center.x << ' ' << arg->clasters[i].center.y << std::endl;
    //     for (auto point : arg->clasters[i].points) {
    //         std::cout << point.x << ' ' << point.y << ' ' << point.claster << std::endl;
    //     }
    // } 
    return NULL;
}


std::vector<Point> make_random_sample(int count, int max_num) {
    std::vector<Point> points(count);
    for (int i = 0; i < count; ++i) {
        points[i].x = std::rand() % max_num;
        points[i].y = std::rand() % max_num;
        points[i].claster = -1;
    }
    return points;
}

double distance(Point lhs, Point rhs) {
    return sqrt((lhs.x - rhs.x) * (lhs.x - rhs.x) +
                (lhs.y - rhs.y) * (lhs.y - rhs.y));
}