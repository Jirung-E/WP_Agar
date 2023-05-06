#pragma once

#include "../Cell.h"


class EnemyCell : public Cell {
public:
    EnemyCell(const Point& position);

public:
    void randomStroll();
};