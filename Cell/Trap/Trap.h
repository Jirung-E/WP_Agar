#pragma warning(disable: 4996)

#include "../Cell.h"


class Trap : public Cell {
private:
    int stroll_count;

public:
    Trap(const Point& position);

public:
    void randomStroll();

    void draw(const HDC& hdc, const Map& map, const RECT& valid_area) const;

    //void move(const Vector& vector, const Map& map);
    void move(const Map& map);

    bool collideWith(const Cell* other);
};