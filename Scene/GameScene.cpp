#include "GameScene.h"

#include "../Util/Util.h"

#include <sstream>


GameScene::GameScene() : Scene { Game }, map { }, player { Point { map.getWidth()/2.0, map.getHeight()/2.0 } } {
    player.color = Green;
}


void GameScene::draw(const HDC& hdc) const {
    drawBackground(hdc, White);

    // Draw Map
    map.draw(hdc, valid_area);

    // Draw Feeds
    for(auto e : feeds) {
        e->draw(hdc, map, valid_area);
    }

    // Draw Enemy


    // Draw Player
    player.draw(hdc, map, valid_area);
    tstring text;
    std::basic_stringstream<TCHAR> ss;
    ss << player.position.x << L"         " << player.position.y;
    text = ss.str();
    TextOut(hdc, 0, 0, text.c_str(), text.length());
    ss.str(L"");
    ss << player.getRadius();
    text = ss.str();
    TextOut(hdc, 0, 16, text.c_str(), text.length());

    // Draw Score

}


void GameScene::update(const POINT& point) {
    POINT p = player.absolutePosition(map, valid_area);
    Vector v = (Point { (double)point.x, (double)point.y } - Point { (double)p.x, (double)p.y }) / 50;
    player.move(v, map);
    player.growUp();

    std::list<Feed*>::iterator iter = feeds.begin();
    for(int i=0; i<feeds.size(); ++i) {
        std::list<Feed*>::iterator next = iter;
        next++;
        if(player.collideWith(*iter)) {
            player.eat(*iter);
            delete *iter;
            feeds.erase(iter);
        }
        iter = next;
    }
}

void GameScene::setUp() {
    player.position = { map.getWidth()/2.0, map.getHeight()/2.0 };
    player.velocity = { 0, 0 };
    feeds.clear();
}


void GameScene::randomGenFeed() {
    if(feeds.size() > 1000) {
        return;
    }

    for(int i=0; i<20; ++i) {
        feeds.push_back(new Feed { Point { getRandomNumberOf(Range { 1.0, (double)map.getWidth()-1 }, 0.1), 
                                           getRandomNumberOf(Range { 1.0, (double)map.getHeight()-1 }, 0.1) } });
    }
}