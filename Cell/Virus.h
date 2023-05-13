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

    //void eat(Cell* cell);                     // �̰͵�...
    void growUp();

    Point getCenterPoint() const;       // ī�޶� �߽�?      �����߽����� �ϴ���, ��¥ �߽����� �ϴ���... �����߽����� �ϸ� ����������, ū������ ī�޶� ��..
    double getRadius() const;           // ī�޶� ����?
    
    double getSize() const;
    // ũ��� ������ ���� ���̷� ����
    // �׷� ������ �������� cell �� ��Ŀ���� �ߴ��Ÿ� ũ�� �������� �ٲ����.

    void split();
};