#pragma once(disable : 4996)

#include "Cell.h"

#include <list>


class Virus {
private:
    int merge_count;

public:
    COLORREF color;
    std::list<Cell*> cells;

public:
    Virus(const Point& position);

public:
    void setUp();

    void draw(const HDC& hdc, const Map& map, const RECT& valid_area) const;

    void move(const Vector& vector, const Map& map);
    void move(const Map& map);

    void update();

    void collideWith(Virus* other);

    //void eat(Cell* cell);                     // 이것도...
    void growUp();

    Point getCenterPoint() const;       // 카메라 중심?      무게중심으로 하는지, 진짜 중심으로 하는지... 무게중심으로 하면 많은쪽으로, 큰쪽으로 카메라가 쏠림..
    double getRadius() const;           // 카메라 영역?
    
    double getSize() const;
    // 크기는 반지름 말고 넓이로 측정
    // 그럼 반지름 제한으로 cell 더 못커지게 했던거를 크기 제한으로 바꿔야함.

    void split();
};