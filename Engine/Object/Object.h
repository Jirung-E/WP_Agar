#pragma once

#include "../Point/Point.h"


class Object {
public:
    Point position;
    Vector velocity;

public:
    Object(const Point& position);
};