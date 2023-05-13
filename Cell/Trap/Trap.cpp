#include "Trap.h"


Trap::Trap(const Point& position, const double radius) : Cell { position, radius } {

}


void Trap::randomStroll() {
    if(stroll_count++ < 5) {
        return;
    }
    velocity = Vector { getRandomNumberOf(Range { -1, 1 }, 0.1), getRandomNumberOf(Range { -1, 1 }, 0.1) };
    if(velocity.scalar() > 1) {
        velocity = velocity.unit();
    }
    else if(velocity.scalar() <= 0.1) {
        velocity = { 0, 0 };
    }
    velocity = velocity/10;
    stroll_count = 0;
}