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
    collisionCheck();
}


void GameScene::updatePlayer(const POINT& point) {
    POINT p = player.absolutePosition(map, valid_area);
    Vector v = (Point { (double)point.x, (double)point.y } - Point { (double)p.x, (double)p.y }) / 50;
    player.move(v, map);
    player.growUp();
}

void GameScene::updateEnemy() {
    for(auto& e : enemies) {
        e->growUp();
        // 진동문제 - 인식 범위를 넘나들면서, 도망갔다가 먹이를 쫓았다가 하면서 진동함
        // 해결방법: 
        // Cell에 변수 추가 - enemy_detected
        // true일때는 먹이를 쫒지 않음
        // (외곽선(중심점에서부터 반지름만큼+)기준)
        // 주변 2칸 이내에 적이 없을시 false가 됨
        // 주변 1.5칸 이내에 적이 있을시 true가 됨

        // 우선순위가 있음
        // 1. 도망
        // 2. 추격
        // 3. 먹이먹기
        // 변수가 따로 있어야함.

        // 인식범위가 다름
        // 도망: 예) 1.5칸에서 인식, 2칸까지 도망
        // 추격: 1.5칸에서 인식, 1.5칸 이하일때만 추격     -> 이렇게 안하면 추격상태일때 인식범위가 넓어져버릴수도 있음. 그렇다고 추격대상 저장하면 대상 없어져버리면 터짐
        // 먹이먹기: 범위제한 없음
        std::list<Cell*> detected_cells;
        double running_range = 2.0;
        double detect_range = 1.5;
        // 범위 안의 적을 다 구함
        if((player.position - e->position).scalar() - player.getRadius() - e->getRadius() <= running_range) {
            detected_cells.push_back(&player);
        }
        for(auto& o : enemies) {
            if(e == o) {
                continue;
            }

            Vector to_other = o->position - e->position;

            if(to_other.scalar() - o->getRadius() - e->getRadius() <= running_range) {
                detected_cells.push_back(o);
            }

            // 도망갈때, 가장 가까운 적이 아니라 범위 안의 모든 적에 대해서 도망을 가야함. 
            // 거리에 따른 가중치를 줄까?

            //if(o->getRadius() >= e->getRadius()) {
            //// 도망갈때는 범위 안의 모든 적으로부터 도망
            //    if(to_other.scalar() - o->getRadius() - e->getRadius() <= (e->running ? 2 : 1.5)) {
            //        e->running = true;
            //        dir -= to_other.unit();
            //    }
            //}
            //else {
            //// 추격할때는 한놈만 추격

            //}

        }

        Vector dir = { 0, 0 };

        if(e->running || e->chasing) {
            e->move(dir, map);
            continue;
        }

        // 먹이를 쫒아감
        Vector to_feed = { 0, 0 };
        double max_dist = 10000000;
        for(auto o : feeds) {
            Vector v = o->position - e->position;
            if(v.scalar() < max_dist) {
                max_dist = v.scalar();
                to_feed = v;
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
    }
}

void GameScene::collisionCheck() {
    playerCollisionCheck();
    enemyCollisionCheck();
}

void GameScene::playerCollisionCheck() {
    // 먹이를 먹음
    std::list<Feed*>::iterator feed_iter = feeds.begin();
    for(int i=0; i<feeds.size(); ++i) {
        std::list<Feed*>::iterator next = feed_iter;
        next++;
        if(player.collideWith(*feed_iter)) {
            player.eat(*feed_iter);
            delete* feed_iter;
            feeds.erase(feed_iter);
        }
        feed_iter = next;
    }

    // 적과 충돌
    std::list<EnemyCell*>::iterator e_iter = enemies.begin();
    for(int i=0; i<enemies.size(); ++i) {
        std::list<EnemyCell*>::iterator next = e_iter;
        next++;
        if(player.collideWith(*e_iter)) {
            if(player.getRadius() > (*e_iter)->getRadius()) {
                player.eat(*e_iter);
                delete* e_iter;
                enemies.erase(e_iter);
            }
            else if(player.getRadius() < (*e_iter)->getRadius()) {
                // 게임오버
            }
        }
        e_iter = next;
    }
}

void GameScene::enemyCollisionCheck() {
    // 적이 먹이를 먹음
    std::list<Feed*>::iterator feed_iter = feeds.begin();
    for(auto& e : enemies) {
        feed_iter = feeds.begin();
        for(int i=0; i<feeds.size(); ++i) {
            std::list<Feed*>::iterator next = feed_iter;
            next++;
            if(e->collideWith(*feed_iter)) {
                e->eat(*feed_iter);
                delete* feed_iter;
                feeds.erase(feed_iter);
            }
            feed_iter = next;
        }
    }

    // 적이 적과 충돌
    std::list<EnemyCell*>::iterator e_iter = enemies.begin();
    for(int i=0; i<enemies.size(); ++i) {
        std::list<EnemyCell*>::iterator other_iter = enemies.begin();
        bool e_iter_deleted = false;
        for(int k=0; k<enemies.size(); ++k) {
            if(other_iter == e_iter) {
                continue;
            }
            bool other_iter_deleted = false;
            if((*e_iter)->collideWith(*other_iter)) {
                if((*e_iter)->getRadius() > (*other_iter)->getRadius()) {
                    std::list<EnemyCell*>::iterator other_next = other_iter;
                    other_next++;
                    (*e_iter)->eat(*other_iter);
                    delete* other_iter;
                    enemies.erase(other_iter);
                    other_iter = other_next;
                    other_iter_deleted = true;
                }
                else if((*e_iter)->getRadius() < (*other_iter)->getRadius()) {
                    std::list<EnemyCell*>::iterator e_next = e_iter;
                    e_next++;
                    (*other_iter)->eat(*e_iter);
                    delete* e_iter;
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
        enemies.push_back(new EnemyCell { Point { getRandomNumberOf(Range { 1.0, (double)map.getWidth()-1 }, 0.1),
                                                  getRandomNumberOf(Range { 1.0, (double)map.getHeight()-1 }, 0.1) } });
        enemies.back()->color = getRandomColor();
    }
}