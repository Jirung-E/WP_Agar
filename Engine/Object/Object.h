#pragma once

#include "../Point/Point.h"


class Object {
public:
    Point position;
    Vector velocity;
    Vector accelerate;

public:
    Object(const Point& position);
};