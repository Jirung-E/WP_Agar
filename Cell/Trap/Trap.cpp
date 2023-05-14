#include "Trap.h"


Trap::Trap(const Point& position) : Cell { position, 0.5 }, stroll_count { 0 }, spawn_animation_count { 0 } {
    color = getRandomColor(0, 0, 0, Range { 100, 200 });
}


void Trap::randomStroll() {
    if(stroll_count++ < 100) {
        return;
    }
    velocity = Vector { getRandomNumberOf(Range { -1, 1 }, 0.1), getRandomNumberOf(Range { -1, 1 }, 0.1) };
    velocity = velocity.unit();
    //velocity = velocity/10;
    stroll_count = 0;
}


void Trap::draw(const HDC& hdc, const Map& map, const RECT& valid_area) const {
    //Cell::draw(hdc, map, valid_area);

    RECT area = map.absoluteArea(valid_area);
    int w = area.right - area.left;
    int h = area.bottom - area.top;
    int x = area.left + position.x / map.getWidth() * w;
    int y = area.top + position.y / map.getHeight() * h;
    int r = radius / map.getWidth() * w;

    HPEN bd_pen = CreatePen(PS_SOLID, 2, color & DarkGray);
    HPEN old_pen = (HPEN)SelectObject(hdc, bd_pen);
    HBRUSH hatch = CreateHatchBrush(HS_DIAGCROSS, Black);
    HBRUSH old = (HBRUSH)SelectObject(hdc, hatch);
    COLORREF c = SetBkColor(hdc, color);

    Ellipse(hdc, x-r, y-r, x+r, y+r);
    
    SetBkColor(hdc, c);
    SelectObject(hdc, old);
    DeleteObject(hatch);
    SelectObject(hdc, old_pen);
    DeleteObject(bd_pen);
}


//void Trap::move(const Vector& vector, const Map& map) {
//    velocity = vector;
//    if(velocity.scalar() > 1) {
//        velocity = velocity.unit();
//    }
//    else if(velocity.scalar() <= 0.1) {
//        velocity = { 0, 0 };
//    }
//    move(map);
//}
//
void Trap::move(const Map& map) {
    Cell::move(map);
    randomStroll();
}


bool Trap::collideWith(const Cell* other) {
    double dist_between_center = Vector(other->position - position).scalar();
    if(dist_between_center <= radius/3*2 + other->getRadius()) {
        return true;
    }
    return false;
}