#include "Cell.h"


Cell::Cell(const Point& position) : Object { position }, radius { 0.3 }, color { White } {

}


void Cell::draw(const HDC& hdc, const Map& map, const RECT& valid_area) const {
    RECT area = map.absoluteArea(valid_area);
    int w = area.right - area.left;
    int h = area.bottom - area.top;
    int x = area.left + position.x / map.getWidth() * w;
    int y = area.top + position.y / map.getHeight() * h;
    int r = radius / map.getWidth() * w;

    HBRUSH bg_br = CreateSolidBrush(color);
    HPEN bd_pen = CreatePen(PS_SOLID, 2, color & DarkGray);
    HBRUSH old_br = (HBRUSH)SelectObject(hdc, bg_br);
    HPEN old_pen = (HPEN)SelectObject(hdc, bd_pen);

    Ellipse(hdc, x-r, y-r, x+r, y+r);

    SelectObject(hdc, old_br);
    SelectObject(hdc, old_pen);
    DeleteObject(bd_pen);
    DeleteObject(bg_br);
}

POINT Cell::absolutePosition(const Map& map, const RECT& valid_area) const {
    RECT area = map.absoluteArea(valid_area);
    int w = area.right - area.left;
    int h = area.bottom - area.top;
    int x = area.left + position.x / map.getWidth() * w;
    int y = area.top + position.y / map.getHeight() * h;
    return { x, y };
}

void Cell::move(const Vector& vector, const Map& map) {
    velocity = vector;
    if(velocity.scalar() > 1) {
        velocity = velocity.unit();
    }
    else if(velocity.scalar() <= 0.1) {
        velocity = { 0, 0 };
    }
    velocity = velocity/10;
    position += velocity;

    if(position.x - radius <= 0) {
        position.x = radius;
        velocity.x = 0;
    }
    else if(position.x + radius >= map.getWidth()) {
        position.x = map.getWidth() - radius;
        velocity.x = 0;
    }
    if(position.y - radius <= 0) {
        position.y = radius;
        velocity.y = 0;
    }
    else if(position.y + radius >= map.getHeight()) {
        position.y = map.getHeight() - radius;
        velocity.y = 0;
    }
}

bool Cell::collideWith(const Cell* other) {
    double dist_between_center = Vector(other->position - position).scalar();
    if(dist_between_center <= radius + other->radius/3) {
        return true;
    }
    return false;
}


void Cell::growUp(double radius) {
    this->radius += radius;
}

double Cell::getRadius() const {
    return radius;
}