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
    for(auto e : enemies) {
        e->draw(hdc, map, valid_area);
    }

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


void GameScene::setUp() {
    player.position = { map.getWidth()/2.0, map.getHeight()/2.0 };
    player.setUp();
    feeds.clear();
    enemies.clear();
}


void GameScene::update(const POINT& point) {
    updatePlayer(point);
    updateEnemy();

    // 먹이를 먹음
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

    // 적과 충돌
    std::list<Cell*>::iterator e_iter = enemies.begin();
    for(int i=0; i<enemies.size(); ++i) {
        std::list<Cell*>::iterator next = e_iter;
        next++;
        if(player.collideWith(*e_iter)) {
            if(player.getRadius() > (*e_iter)->getRadius()) {
                player.eat(*e_iter);
                delete* e_iter;
                enemies.erase(e_iter);
            }
        }
        e_iter = next;
    }

    // 적이 먹이를 먹음
    for(auto& e : enemies) {
        iter = feeds.begin();
        for(int i=0; i<feeds.size(); ++i) {
            std::list<Feed*>::iterator next = iter;
            next++;
            if(e->collideWith(*iter)) {
                e->eat(*iter);
                delete *iter;
                feeds.erase(iter);
            }
            iter = next;
        }
    }

    // 적이 적과 충돌
    e_iter = enemies.begin();
    for(int i=0; i<enemies.size(); ++i) {
        std::list<Cell*>::iterator other_iter = enemies.begin();
        bool e_iter_deleted = false;
        for(int k=0; k<enemies.size(); ++k) {
            if(other_iter == e_iter) {
                continue;
            }
            bool other_iter_deleted = false;
            if((*e_iter)->collideWith(*other_iter)) {
                if((*e_iter)->getRadius() > (*other_iter)->getRadius()) {
                    std::list<Cell*>::iterator other_next = other_iter;
                    other_next++;
                    (*e_iter)->eat(*other_iter);
                    delete *other_iter;
                    enemies.erase(other_iter);
                    other_iter = other_next;
                    other_iter_deleted = true;
                }
                else if((*e_iter)->getRadius() < (*other_iter)->getRadius()) {
                    std::list<Cell*>::iterator e_next = e_iter;
                    e_next++;
                    (*other_iter)->eat(*e_iter);
                    delete *e_iter;
                    enemies.erase(e_iter);
                    e_iter = e_next;
                    e_iter_deleted = true;
                    break;
                }
            }
            if(!other_iter_deleted) {
                other_iter++;
            }
            other_iter_deleted = false;
        }
        if(!e_iter_deleted) {
            e_iter++;
        }
        e_iter_deleted = false;
    }
}


void GameScene::updatePlayer(const POINT& point) {
    POINT p = player.absolutePosition(map, valid_area);
    Vector v = (Point { (double)point.x, (double)point.y } - Point { (double)p.x, (double)p.y }) / 50;
    player.move(v, map);
    player.growUp();
}

void GameScene::updateEnemy() {
    for(auto& e : enemies) {
        Vector to_feed = { 0, 0 };
        double max_dist = 10000000;
        for(auto o : feeds) {
            Vector _v = o->position - e->position;
            if(_v.scalar() < max_dist) {
                max_dist = _v.scalar();
                to_feed = _v;
            }
        }
        to_feed = to_feed.unit() * 10;
        if(to_feed == Vector { 0, 0 }) {
            e->randomStroll();
            e->move(map);
        }
        else {
            e->move(to_feed, map);
        }
        e->growUp();
    }
}

void GameScene::collisionCheck() {

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

void GameScene::randomGenEnemy() {
    if(enemies.size() > 10) {
        return;
    }

    for(int i=0; i<3; ++i) {
        enemies.push_back(new Cell { Point { getRandomNumberOf(Range { 1.0, (double)map.getWidth()-1 }, 0.1),
                                             getRandomNumberOf(Range { 1.0, (double)map.getHeight()-1 }, 0.1) } });
        enemies.back()->color = getRandomColor();
    }

    //enemyRandomMove();
}


//void GameScene::enemyRandomMove() {
//    for(auto& e : enemies) {
//        e->randomStroll();
//    }
//}