#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>

struct Point {
    int x;
    int y;
    int claster; // == -1 : не привязана, == i: привязана к i кластеру
};

std::vector<Point> make_random_sample(int count, int max_num) {
    std::vector<Point> points(count);
    for (int i = 0; i < count; ++i) {
        points[i].x = std::rand() % max_num;
        points[i].y = std::rand() % max_num;
        points[i].claster = -1;
    }
    return points;
}


int main() {
    int const MAX_COORD = 250;
    int const POINTS_COUNT = 100;
    std::srand(std::time(nullptr));

    std::vector<Point> points = make_random_sample(POINTS_COUNT, MAX_COORD);

    std::ofstream out;
    out.open("sample.txt");
    if (out.is_open()) {
        // Записываем в файл
        out << POINTS_COUNT << ' ' << MAX_COORD << std::endl;
        for (auto point : points) {
            out << point.x << ' ' << point.y << ' ' << point.claster << std::endl;
        }
    }
    out.close();

    return 0;
}