#include "GameScene.h"

#include "../Util/Util.h"

#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>


GameScene::GameScene() : Scene { Game }, map { },
player { Point { map.getWidth()/2.0, map.getHeight()/2.0 } }, paused { false }, cam_mode { Fixed }, show_score { false },
resume_button { L"Resume", { 20, 30 }, 60, 15 }, quit_button { L"Quit", { 20, 60 }, 60, 15 }, 
game_over_message { L"Game Over", { 10, 30 }, 80, 15 }, game_over { false },
play_time { 0 }, start_time { clock() }, end_time { clock() } {
    player.color = Green;
    resume_button.border_color = Gray;
    resume_button.border_width = 3;
    resume_button.id = ResumeGame;
    quit_button.border_color = Gray;
    quit_button.border_width = 3;
    quit_button.id = QuitGame;
    game_over_message.text_color = Red;
    game_over_message.background_color = LightGray;
    game_over_message.bold = 4;
}


void GameScene::setUp() {
    player.position = { map.getWidth()/2.0, map.getHeight()/2.0 };
    player.setUp();
    feeds.clear();
    enemies.clear();
    paused = false;
    game_over = false;
    show_score = false;
    play_time = 0;
    start_time = clock();
    end_time = clock();
    feed_erase_count = 0;
}


void GameScene::update(const POINT& point) {
    if(paused) {
        return;
    }
    if(!game_over) {
        end_time = clock();
        play_time += end_time - start_time;
        start_time = clock();
        updatePlayer(point);
    }
    updateEnemy();
    collisionCheck();
}

void GameScene::pause() {
    if(!game_over) {
        paused = true;
    }
}

void GameScene::resume() {
    if(!game_over) {
        paused = false;
        end_time = clock();
        start_time = clock();
    }
}


void GameScene::updatePlayer(const POINT& point) {
    POINT p;
    switch(cam_mode) {
    case Fixed:
        p = player.absolutePosition(map, valid_area);
        break;
    case Dynamic:
        p = { (valid_area.left + valid_area.right)/2, (valid_area.top + valid_area.bottom)/2 };
        break;
    }
    Vector v = (Point { (double)point.x, (double)point.y } - Point { (double)p.x, (double)p.y }) / 50;
    player.move(v, map);
    player.growUp();
}

void GameScene::updateEnemy() {
    for(auto& e : enemies) {
        e->growUp();

        std::list<Cell*> detected_cells;
        const double running_range = 2;
        const double detect_range = 1.5;
        double range = e->running ? running_range : detect_range;

        // 범위 안의 적을 다 구함
        if(!game_over) {
            if((player.position - e->position).scalar() - player.getRadius() - e->getRadius() <= range) {
                detected_cells.push_back(&player);
            }
        }
        for(auto& o : enemies) {
            if(e == o) {
                continue;
            }
            if((o->position - e->position).scalar() - o->getRadius() - e->getRadius() <= range) {
                detected_cells.push_back(o);
            }
        }

        e->running = false;
        e->chasing = false;
        Vector dir = { 0, 0 };
        Cell* target = nullptr;
        double max_r = 0;
        for(auto o : detected_cells) {
            Vector to_me = e->position - o->position;
            double dist = to_me.scalar() - e->getRadius() - o->getRadius();
            if(o->getRadius() >= e->getRadius()) {
                // 도망갈준비
                dir += to_me.unit() / (to_me.scalar() + dist);
                e->running = true;
            }
            else {
                // 쫒아갈준비
                if(dist <= detect_range) {
                    if(o->getRadius() > max_r) {
                        max_r = o->getRadius();
                        target = o;
                        e->chasing = true;
                    }
                }
            }
        }
        if(e->running) {
            e->move(dir.unit(), map);
            continue;
        }
        if(e->chasing) {
            e->move((target->position - e->position).unit(), map);
            continue;
        }

        // 먹이를 쫒아감
        Vector to_feed = { 0, 0 };
        double min_dist = 10000000;
        for(auto o : feeds) {
            Vector v = o->position - e->position;
            if(v.scalar() < min_dist) {
                min_dist = v.scalar();
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
    if(!game_over) {
        playerCollisionCheck();
    }
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
                game_over = true;
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


void GameScene::draw(const HDC& hdc) const {
    drawBackground(hdc, White);

    RECT view_area = getViewArea();

    map.draw(hdc, view_area);
    for(auto e : feeds) {
        e->draw(hdc, map, view_area);
    }
    for(auto e : enemies) {
        e->draw(hdc, map, view_area);
    }
    if(!game_over) {
        player.draw(hdc, map, view_area);
    }

    // 플레이어 이동방향
    if(!game_over) {
        Vector v = player.velocity * (view_area.right-view_area.left)/map.getWidth() * player.getRadius()*2;
        if(v.scalar() != 0) {
            HPEN pen = CreatePen(PS_SOLID, (view_area.right-view_area.left)/map.getWidth() * player.getRadius() / 3, LightGray);
            HPEN old = (HPEN)SelectObject(hdc, pen);

            POINT p = player.absolutePosition(map, view_area);
            MoveToEx(hdc, p.x+v.x, p.y+v.y, NULL);
            Vector v1 = { -v.x/3, -v.y/3 };
            double th = atan(v.y/v.x);
            if(v.x < 0) th += M_PI;
            LineTo(hdc, p.x+v.x - v1.scalar()*cos(-M_PI/4 + th), p.y+v.y - v1.scalar()*sin(-M_PI/4 + th));
            MoveToEx(hdc, p.x+v.x, p.y+v.y, NULL);
            LineTo(hdc, p.x+v.x - v1.scalar()*cos(M_PI/4 + th), p.y+v.y - v1.scalar()*sin(M_PI/4 + th));

            SelectObject(hdc, old);
            DeleteObject(pen);
        }
    }

    if(show_score) {
        drawScore(hdc);
    }

    if(paused) {
        drawPauseScene(hdc);
    }
    else if(game_over) {
        drawGameOverScene(hdc);
    }
}

RECT GameScene::getViewArea() const {
    switch(cam_mode) {
    case Fixed:
        return valid_area;
    case Dynamic:
        {
            // 보이는 영역의 크기 설정
            double r0 = player.min_radius;
            double rm = player.max_radius;
            double w = horizontal_length/map.getWidth() * (-(2*rm-15*r0) / pow(r0-rm, 2) * pow(player.getRadius() - rm, 2) + 2*rm);
            double W = valid_area.left + horizontal_length/2;
            double p = W*(W/w - 1);

            // 플레이어가 중앙에 오도록 평행이동
            Point pp = player.position;
            pp += Vector { -map.getWidth()/2.0, -map.getHeight()/2.0 };
            double px = W/w * horizontal_length/map.getWidth() * pp.x;
            double py = W/w * horizontal_length/map.getWidth() * pp.y;

            return {
                LONG(floor(valid_area.left - px - p)),
                LONG(floor(valid_area.top - py - p)),
                LONG(floor(valid_area.right - px + p)),
                LONG(floor(valid_area.bottom - py + p))
            };
        }
    }
    return valid_area;
}

void GameScene::drawScore(const HDC& hdc) const {
    TextBox score { L"", { 0, 0 }, 50, 5 };
    score.transparent_background = true;
    score.transparent_border = true;
    score.align = DT_LEFT;
    score.square = false;
    score.bold = 4;
    score.italic = true;

    tstring text;
    std::basic_stringstream<TCHAR> ss;

    // 반지름 출력
    ss << L"Size: " << player.getRadius() * 10;
    text = ss.str();
    //TextOut(hdc, 2, 2, text.c_str(), text.length());
    score.text = ss.str();
    score.show(hdc, valid_area);
    ss.str(L"");

    // 플레이 시간 출력
    ss << L"PlayTime: " << play_time/1000 << "\"";
    text = ss.str();
    //TextOut(hdc, 2, 18, text.c_str(), text.length());
    score.text = ss.str();
    score.position.y += 5;
    score.show(hdc, valid_area);
    ss.str(L"");
}

void GameScene::drawPauseScene(const HDC& hdc) const {
    resume_button.show(hdc, valid_area);
    quit_button.show(hdc, valid_area);
}

void GameScene::drawGameOverScene(const HDC& hdc) const {
    game_over_message.show(hdc, valid_area);
    quit_button.show(hdc, valid_area);

    TextBox score { L"", { 0, 45 }, 100, 7 };
    score.transparent_background = true;
    score.transparent_border = true;
    score.bold = 4;

    tstring text;
    std::basic_stringstream<TCHAR> ss;

    // 반지름 출력
    ss << L"Size: " << player.getRadius() * 10;
    text = ss.str();
    score.text = ss.str();
    score.show(hdc, valid_area);
    ss.str(L"");

    // 플레이 시간 출력
    ss << L"PlayTime: " << play_time/1000 << "\"";
    text = ss.str();
    score.text = ss.str();
    score.position.y += 7;
    score.show(hdc, valid_area);
    ss.str(L"");
}


void GameScene::setCameraMode(const CameraMode& mode) {
    if(paused) {
        return;
    }
    cam_mode = mode;
}

void GameScene::randomGenFeed() {
    if(paused) {
        return;
    }

    feed_erase_count++;
    if(feeds.size() > 200 || feed_erase_count >= 7) {
        feed_erase_count = 0;
        // 오래된거(앞에 10개) 제거
        for(int i=0; i<10; ++i) {
            if(feeds.size() > 0) {
                feeds.pop_front();
            }
        }
    }

    for(int i=0; i<20; ++i) {
        feeds.push_back(new Feed { Point { getRandomNumberOf(Range { 1.0, (double)map.getWidth()-1 }, 0.1), 
                                           getRandomNumberOf(Range { 1.0, (double)map.getHeight()-1 }, 0.1) } });
    }
}

void GameScene::randomGenEnemy() {
    if(paused) {
        return;
    }

    if(enemies.size() > 10) {
        return;
    }

    for(int i=0; i<1; ++i) {
        enemies.push_back(new EnemyCell { Point { getRandomNumberOf(Range { 1.0, (double)map.getWidth()-1 }, 0.1),
                                                  getRandomNumberOf(Range { 1.0, (double)map.getHeight()-1 }, 0.1) } });
        enemies.back()->color = getRandomColor();
    }
}


ButtonID GameScene::click(const POINT& point) const {
    if(paused) {
        RECT r = resume_button.absoluteArea(valid_area);
        if(PtInRect(&r, point)) {
            return resume_button.id;
        }
        r = quit_button.absoluteArea(valid_area);
        if(PtInRect(&r, point)) {
            return quit_button.id;
        }
        return None;
    }
    if(game_over) {
        RECT r = quit_button.absoluteArea(valid_area);
        if(PtInRect(&r, point)) {
            return quit_button.id;
        }
        return None;
    }
    return None;
}