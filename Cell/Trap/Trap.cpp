#include "Trap.h"


Trap::Trap(const Point& position) : Cell { position, 0.6 }, stroll_count { 0 }, spawn_animation_count { 0 } {
    invincible = true;
    color = getRandomColor(0, 0, 0, Range { 50, 100 });
}


void Trap::randomStroll() {
    if(stroll_count++ < 150) {
        return;
    }
    velocity = Vector { getRandomNumberOf(Range { -1, 1 }, 0.1), getRandomNumberOf(Range { -1, 1 }, 0.1) };
    velocity = velocity.unit();
    velocity = velocity/2;
    stroll_count = 0;
}


void Trap::draw(const HDC& hdc, const Map& map, const RECT& valid_area) {
    if(spawn_animation_count++ < 1000) {

    }

    RECT area = map.absoluteArea(valid_area);
    int w = area.right - area.left;
    int h = area.bottom - area.top;
    int x = area.left + position.x / map.getWidth() * w;
    int y = area.top + position.y / map.getHeight() * h;
    int r = radius / map.getWidth() * w;

    HPEN bd_pen = CreatePen(PS_SOLID, 2, color & DarkGray);
    HPEN old_pen = (HPEN)SelectObject(hdc, bd_pen);
    HBRUSH br = CreateHatchBrush(HS_DIAGCROSS, Black);
    HBRUSH old_br = (HBRUSH)SelectObject(hdc, br);
    COLORREF old_c = SetBkColor(hdc, color);

    Ellipse(hdc, x-r, y-r, x+r, y+r);
    
    SetBkColor(hdc, old_c);
    SelectObject(hdc, old_br);
    DeleteObject(br);
    SelectObject(hdc, old_pen);
    DeleteObject(bd_pen);
}


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