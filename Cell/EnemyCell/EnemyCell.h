#pragma once

#include "../Cell.h"


class EnemyCell : public Cell {
private:
    int stroll_count;

public:
    bool running;
    bool chasing;

public:
    EnemyCell(const Point& position);

public:
    void randomStroll();
};