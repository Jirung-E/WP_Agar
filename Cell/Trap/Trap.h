#pragma warning(disable: 4996)

#include "../Cell.h"


class Trap : public Cell {
private:
    int stroll_count;

public:
    Trap(const Point& position, const double radius = 0.1);

public:
    void move(const Map& map);
    void randomStroll();
};