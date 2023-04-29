#include "Feed.h"


Feed::Feed(const Point& position) : Cell { position } {
    radius = 0.2;
    color = getRandomColor();
}