#pragma once

#include "../Engine/Object/Object.h"

#include "../Map/Map.h"


class Cell : public Object {
public:
    COLORREF color;

protected:
    double radius;

public:
    Cell(const Point& position);

public:
    void draw(const HDC& hdc, const Map& map, const RECT& valid_area) const;
    POINT absolutePosition(const Map& map, const RECT& valid_area) const;
    void move(const Vector& vector, const Map& map);
    bool collideWith(const Cell* other);
    void growUp(double radius);
    double getRadius() const;
};