#include "Cell.h"


Cell::Cell(const Point& position) : Object { position }, radius { 0.3 }, color { White }, 
target_radius { radius }, prev_radius { radius }, trans_count { 0 }, max_radius { 5 } {

}


void Cell::setUp() {
    radius = 0.3;
    target_radius = radius;
    prev_radius = radius;
    trans_count = 0;
    velocity = { 0, 0 };
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
    velocity /= 10 * (0.7+radius);
    move(map);
}

void Cell::move(const Map& map) {
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
    if(radius >= other->radius) {
        if(dist_between_center <= radius + other->radius/3) {
            return true;
        }
    }
    else {
        if(dist_between_center <= radius/3 + other->radius) {
            return true;
        }
    }
    return false;
}

void Cell::eat(Cell* cell) {
    target_radius = sqrt(pow(target_radius, 2) + pow(cell->radius, 2));
    if(target_radius > max_radius) {
        target_radius = max_radius;
    }
    prev_radius = radius;
    trans_count = 0;
}

void Cell::growUp() {
    if(trans_count > 60) {
        return;
    }
    radius = -(target_radius-prev_radius)*pow(trans_count++/60.0-1, 6) + target_radius;
}

double Cell::getRadius() const {
    return radius;
}