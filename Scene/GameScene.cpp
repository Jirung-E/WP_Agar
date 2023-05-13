#include "GameScene.h"

#include "../Util/Util.h"

#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>


GameScene::GameScene() : Scene { Game }, map { },
player { Point { map.getWidth()/2.0, map.getHeight()/2.0 } }, paused { false }, cam_mode { Fixed }, show_score { false },
resume_button { L"Resume", { 20, 30 }, 60, 15 }, quit_button { L"Quit", { 20, 60 }, 60, 15 }, 
game_over_message { L"Game Over", { 10, 30 }, 80, 15 }, game_over { false }, feed_erase_count { 0 },
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
    player.cells.clear();
    player.cells.push_back(new Cell { Point { map.getWidth()/2.0, map.getHeight()/2.0 } });
    player.cells.front()->color = player.color;
    //player.cells.front()->position = { map.getWidth()/2.0, map.getHeight()/2.0 };
    //player.setUp();
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
    updateFeeds();
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
    RECT view_area = getViewArea();

    for(auto e : player.cells) {
        POINT p;
        switch(cam_mode) {
        case Fixed:
            p = e->absolutePosition(map, valid_area);
            break;
        case Dynamic:
            POINT center = { (valid_area.left + valid_area.right)/2, (valid_area.top + valid_area.bottom)/2 };
            Vector v = (e->position - player.getCenterPoint()) * (view_area.right-view_area.left) / map.getWidth();
            p = { LONG(center.x + v.x), LONG(center.y + v.y) };
            break;
        }
        Vector dir = (Point { (double)point.x, (double)point.y } - Point { (double)p.x, (double)p.y }) / 50;
        e->move(dir, map);
        e->growUp();
    }
}

void GameScene::updateEnemy() {
    for(auto& e : enemies) {
        e->growUp();

        std::list<Cell*> detected_cells;
        const double running_range = 2;
        const double detect_range = 1.5;
        double range = e->running ? running_range : detect_range;

        // ���� ���� ���� �� ����
        if(!game_over) {
            for(auto p : player.cells) {
                if((p->position - e->position).scalar() - p->getRadius() - e->getRadius() <= range) {
                    detected_cells.push_back(p);
                }
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
                // �������غ�
                dir += to_me.unit() / (to_me.scalar() + dist);
                e->running = true;
            }
            else {
                // �i�ư��غ�
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

        // ���̸� �i�ư�
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

void GameScene::updateFeeds() {
    for(auto e : feeds) {
        if(e->velocity == Vector { 0, 0 }) {
            continue;
        }
        e->move(map);
    }
}

void GameScene::collisionCheck() {
    if(!game_over) {
        playerCollisionCheck();
    }
    enemyCollisionCheck();
}

void GameScene::playerCollisionCheck() {
    player.update();

    // ���̸� ����
    std::list<Feed*>::iterator feed_iter = feeds.begin();
    for(int i=0; i<feeds.size(); ++i) {
        std::list<Feed*>::iterator next = feed_iter;
        next++;
        for(auto e : player.cells) {
            if(e->collideWith(*feed_iter)) {
                e->eat(*feed_iter);
                delete* feed_iter;
                feeds.erase(feed_iter);
                break;
            }
        }
        feed_iter = next;
    }

    // ���� �浹
    std::list<EnemyCell*>::iterator e_iter = enemies.begin();
    for(int i=0; i<enemies.size(); ++i) {
        std::list<EnemyCell*>::iterator next = e_iter;
        next++;

        std::list<Cell*>::iterator p_iter = player.cells.begin();
        for(int i=0; i<player.cells.size(); ++i) {
            std::list<Cell*>::iterator p_next = p_iter;
            p_next++;

            if((*p_iter)->collideWith(*e_iter)) {
                if((*p_iter)->getRadius() >(*e_iter)->getRadius()) {
                    (*p_iter)->eat(*e_iter);
                    delete *e_iter;
                    enemies.erase(e_iter);
                }
                else if((*p_iter)->getRadius() < (*e_iter)->getRadius()) {
                    (*e_iter)->eat(*p_iter);
                    if(player.cells.size() > 1) {
                        delete *p_iter;
                        player.cells.erase(p_iter);
                    }
                    else {
                        // ���ӿ���
                        game_over = true;
                    }
                }
                break;
            }

            p_iter = p_next;
        }

        e_iter = next;
    }
}

void GameScene::enemyCollisionCheck() {
    // ���� ���̸� ����
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

    // ���� ���� �浹
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

    // �÷��̾� �̵�����
    if(!game_over) {
        for(auto e : player.cells) {
            Vector pv = e->velocity;
            if(pv.scalar() > 1) {
                pv = pv.unit();
            }
            Vector v = pv * (view_area.right-view_area.left)/map.getWidth() * e->getRadius()*2;
            if(v.scalar() != 0) {
                HPEN pen = CreatePen(PS_SOLID, (view_area.right-view_area.left)/map.getWidth() * e->getRadius() / 3, LightGray);
                HPEN old = (HPEN)SelectObject(hdc, pen);

                POINT p = e->absolutePosition(map, view_area);
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
            // ���̴� ������ ũ�� ����
            double r0 = Cell::min_radius;
            //double rm = Cell::max_radius;
            double rm = sqrt(pow(horizontal_length/map.getWidth(), 2) + pow(vertical_length/map.getHeight(), 2))/2;
            double w = horizontal_length/map.getWidth() * (-(rm-20*r0) / pow(r0-rm, 2) * pow(sqrt(player.getSize()) - rm, 2) + rm);
            //double w = horizontal_length/map.getWidth() * (10*r0*pow(M_E, -(player.getRadius()-r0)) + 10*player.getRadius());
            double W = valid_area.left + horizontal_length/2.0;
            double A = W*(W/w - 1);

            // �÷��̾ �߾ӿ� ������ �����̵�
            Point p = player.getCenterPoint();
            p += Vector { -map.getWidth()/2.0, -map.getHeight()/2.0 };
            double px = W/w * horizontal_length/map.getWidth() * p.x;
            double py = W/w * horizontal_length/map.getWidth() * p.y;

            return {
                LONG(floor(valid_area.left - px - A)),
                LONG(floor(valid_area.top - py - A)),
                LONG(floor(valid_area.right - px + A)),
                LONG(floor(valid_area.bottom - py + A))
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

    // ũ�� ���
    ss << L"Size: " << player.getSize() * 10;
    text = ss.str();
    //TextOut(hdc, 2, 2, text.c_str(), text.length());
    score.text = ss.str();
    score.show(hdc, valid_area);
    ss.str(L"");

    // �÷��� �ð� ���
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

    // ũ�� ���
    ss << L"Size: " << player.getSize() * 10;
    text = ss.str();
    score.text = ss.str();
    score.show(hdc, valid_area);
    ss.str(L"");

    // �÷��� �ð� ���
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
        // �����Ȱ�(�տ� 10��) ����
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


ButtonID GameScene::clickL(const POINT& point) {
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

    player.split();

    return None;
}

ButtonID GameScene::clickR(const POINT& point) {
    if(paused) {
        return None;
    }
    if(game_over) {
        return None;
    }

    for(auto e : player.cells) {
        Cell* c = e->spit();
        if(c != nullptr) {
            Feed* f = new Feed { c->position, c->getRadius() };
            f->color = c->color;
            f->position = c->position;
            f->velocity = c->velocity;
            feeds.push_back(f);
            delete c;
        }
    }

    return None;
}