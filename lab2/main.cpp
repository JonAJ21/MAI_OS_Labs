#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>
#include <pthread.h>

int const MAX_COORD = 20; // Задаю квадрат 20x20
int const POINTS_COUNT = 30; // Задаю количество точек

// using Point = std::pair<int, int>;
struct Point {
    int x;
    int y;
    int claster; // == -1 : не привязана, == i: привязана к i кластеру
};

struct Claster {
    Point center;
    std::vector<Point> points;
};  

// Делаю случайную выбоку точек
std::vector<Point> make_random_sample(int count, int max_num) {
    // std::srand(std::time(nullptr));
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

int main() {
    std::srand(std::time(nullptr));
    std::vector<Point> points = make_random_sample(POINTS_COUNT, MAX_COORD);

    int k = 3; // Число кластеров
    int steps = 30; // Количество итераций, за которое мы будем искать кластеры
    std::vector<Claster> clasters(k); // Наши кластеры
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
    for (int step = 0; step < steps; ++step) {
        // Очищаем точки принадлежащие кластеру
        for (int i = 0; i < k; ++i) {
            clasters[i].points.clear();
        }

        // Сначала мы «привязываем» каждую точку к тому центроиду, к которому она ближе
        for (int j = 0; j < POINTS_COUNT; ++j) {
            int closest_claster_id = 0;
            double closest_distance = 1e9;
            for (int i = 0; i < k; ++i) {
                if (distance(clasters[i].center, points[j]) < closest_distance) {
                    closest_distance = distance(clasters[i].center, points[j]);
                    closest_claster_id = i;
                }
            }
            points[j].claster = closest_claster_id;
            clasters[closest_claster_id].points.push_back(points[j]);
        }

        // Передаем начальные кластеры в файл
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
        for (int i = 0; i < k; ++i) {
            Point new_claster_center = {0, 0, i};
            for (auto point : clasters[i].points) {
                new_claster_center.x += point.x;
                new_claster_center.y += point.y;
            }
            // Проверка выделили мы все кластеры (Если нет, то рандомим все центры еще раз)
            if (clasters[i].points.size() == 0) {
                std::cout << "ZEERROORORO" << std::endl;
                for (int j = 0; j < k; ++j) {
                    new_claster_center.x = std::rand() % MAX_COORD;
                    new_claster_center.y = std::rand() % MAX_COORD;
                    clasters[j].center = new_claster_center;
                }
                break;
            }
            
            new_claster_center.x /= clasters[i].points.size();
            new_claster_center.y /= clasters[i].points.size();
            clasters[i].center = new_claster_center;
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