#pragma once

#include "../Engine/Object/Object.h"

#include "../Map/Map.h"


class Cell : public Object {
public:
    COLORREF color;

protected:
    double radius;
    double target_radius;
    double prev_radius;
    int trans_count;
    const double max_radius;

public:
    Cell(const Point& position);

public:
    void setUp();

    void draw(const HDC& hdc, const Map& map, const RECT& valid_area) const;
    POINT absolutePosition(const Map& map, const RECT& valid_area) const;

    void move(const Vector& vector, const Map& map);
    void move(const Map& map);

public:
    bool collideWith(const Cell* other);

    void eat(Cell* cell);
    void growUp();

    double getRadius() const;
};