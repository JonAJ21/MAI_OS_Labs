#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include <pthread.h>
#include <unistd.h>

int const MAX_COORD = 10000; // Задаю квадрат 20x20
int const POINTS_COUNT = 1000; // Задаю количество точек
int const STEPS = 100; // Количество итераций, за которое мы будем искать кластеры
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

struct Args {
    int thread_id;
    std::vector<Point> *points;
    std::vector<Claster> *clasters;
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
    int k = 5; // Число кластеров
    // std::cout << "Введите количество кластеров" << std::endl;
    // std::cin >> k;

    std::srand(std::time(nullptr));
    pthread_t threads[MAX_THREADS];
    std::vector<Point> points = make_random_sample(POINTS_COUNT, MAX_COORD);
    std::vector<Claster> clasters(k);
    std::vector<Args> args(MAX_THREADS);
    for (int i = 0; i < MAX_THREADS; ++i) {
        args[i] = {i, &points, &clasters};
    }

    // Сначала случайным образом выставляем на этой же плоскости k центров кластеров
    for (int i = 0; i < k; ++i) {
        clasters[i].center.x = std::rand() % MAX_COORD;
        clasters[i].center.y = std::rand() % MAX_COORD;
        clasters[i].center.claster = i;
    }

    // Записываем начальные координаты в файл (для дальнейшей визуализации)
    std::ofstream out;
    out.open("out.txt");
    if (out.is_open()) {
        // Записываем в файл
        out << MAX_COORD << ' ' << POINTS_COUNT << std::endl;
        out << k << std::endl;
    }
    
    // Запускаем цикл поиска кластеров
    for (int step = 0; step < STEPS; ++step) {
        // Очищаем точки принадлежащие кластеру
        for (int i = 0; i < k; ++i) {
            clasters[i].points.clear();
        }
        // Сначала мы «привязываем» каждую точку к тому центроиду, к которому она ближе
        // Надо делать параллельно
        for (int i = 0; i < MAX_THREADS; ++i) {
            if (pthread_create(&threads[i], NULL, connect_point, (void*) &args[i]) != 0) {
                std::cout << "Error: thread did not create " << i << std::endl;
                return 1;
            }
            // std::cout << "Created " << idx << std::endl;
        }
        for (int i = 0; i < MAX_THREADS; ++i) {
            if (pthread_join(threads[i], NULL) != 0) {
                std::cout << "Error: thread did not join " << i << std::endl;
                return 2;
            }
            // std::cout << "Joined " << idx << std::endl;
        }
        
        // Передаем начальные кластеры в файл
        // Параллельно делать не буду т.к может неправильно записаться в файл,
        // можно сделать MUTEX, но есть ли в этом смысл
        if (step == 0) {
            for (int i = 0; i < k; ++i) {
                out << i << ' ' << clasters[i].points.size() << ' ' <<
                clasters[i].center.x << ' ' << clasters[i].center.y << std::endl;
                for (auto point : clasters[i].points) {
                    out << point.x << ' ' << point.y << std::endl;
                }
            }
        }
        

        // Ищем новый центр масс кластера
        // Надо делать параллельно
        for (int i = 0; i < k; ++i) {
            clasters[i].center = {0, 0, i};
        }
        for (int i = 0; i < MAX_THREADS; ++i) {
            if (pthread_create(&threads[i], NULL, find_center, (void*) &args[i]) != 0) {
                std::cout << "Error: thread did not create " << i << std::endl;
                return 3;
            }
            // std::cout << "Created " << idx << std::endl;
        }
        for (int i = 0; i < MAX_THREADS; ++i) {
            if (pthread_join(threads[i], NULL) != 0) {
                std::cout << "Error: thread did not join " << i << std::endl;
                return 4;
            }
            // std::cout << "Joined " << idx << std::endl;
        }
        
        // Проверка выделили мы все кластеры (Если нет, то рандомим все центры еще раз)
        int flag = 0;
        for (int i = 0; i < k; ++i) {
            if (clasters[i].points.size() == 0) {
                for (int j = 0; j < k; ++j) {
                    clasters[j].center.x = std::rand() % MAX_COORD;
                    clasters[j].center.y = std::rand() % MAX_COORD;
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
            clasters[i].center.x /= clasters[i].points.size();
            clasters[i].center.y /= clasters[i].points.size();
        }
    }

    // Передаем финальные кластеры в файл
    for (int i = 0; i < k; ++i) {
        out << i << ' ' << clasters[i].points.size() << ' ' <<
        clasters[i].center.x << ' ' << clasters[i].center.y << std::endl;
        for (auto point : clasters[i].points) {
            out << point.x << ' ' << point.y << std::endl;
        }
    }
 

    out.close();
    return 0;
}

void* connect_point(void* args) {
    Args *arg = (Args*) args;
    int thread_id = arg->thread_id;
    int points_cnt = (int)(*arg->points).size();
    // std::cout << "DEBUG(connect): " << thread_id << std::endl;
    for (int j = thread_id; j < points_cnt; j += MAX_THREADS) {
        int closest_claster_id = 0;
        double closest_distance = 1e9;
        for (int i = 0; i < (int)(*arg->clasters).size(); ++i) {
            if (distance((*arg->clasters)[i].center, (*arg->points)[j]) < closest_distance) {
                closest_distance = distance((*arg->clasters)[i].center, (*arg->points)[j]);
                closest_claster_id = i;
            }
        }
        (*arg->points)[j].claster = closest_claster_id;
        (*arg->clasters)[closest_claster_id].points.push_back((*arg->points)[j]);
    }
    return NULL;
}

void* find_center(void* args) {
    Args *arg = (Args*) args;
    int thread_id = arg->thread_id;
    // std::cout << "DEBUG(center): " << thread_id << std::endl;
    for (int i = 0; i < (int)(*arg->clasters).size(); ++i) {
        int points_cnt = (int) (*arg->clasters)[i].points.size();
        for (int j = thread_id; j < points_cnt; j += MAX_THREADS) {
            (*arg->clasters)[i].center.x += (*arg->clasters)[i].points[j].x;
            (*arg->clasters)[i].center.y += (*arg->clasters)[i].points[j].y;
        }
    }
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