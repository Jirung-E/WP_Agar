#pragma once

#include "../Engine/Object/Object.h"

#include "../Map/Map.h"


class Cell : public Object {
public:
    COLORREF color;
    static const double max_radius;
    static const double min_radius;

protected:
    double radius;
    double target_radius;
    double prev_radius;
    int trans_count;
    int accel_count;

public:
    Cell(const Point& position, const double radius = 0.3);

public:
    void setUp();

    void draw(const HDC& hdc, const Map& map, const RECT& valid_area) const;
    POINT absolutePosition(const Map& map, const RECT& valid_area) const;

    void move(const Vector& vector, const Map& map);
    void move(const Map& map);

    bool collideWith(const Cell* other);

    void eat(Cell* cell);
    void growUp();

    double getRadius() const;

    Cell* spit();
    Cell* split();
};