#include "Feed.h"


Feed::Feed(const Point& position) : Cell { position } {
    radius = 0.1;
    color = getRandomColor();
}