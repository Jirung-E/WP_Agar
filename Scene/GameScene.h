#pragma once

#include "Scene.h"
#include "../Map/Map.h"
#include "../Cell/Cell.h"
#include "../Feed/Feed.h"

#include <list>


class GameScene : public Scene {
private:
    Map map;
    Cell player;
    //std::list<Cell*> enemies;
    std::list<Feed*> feeds;

public:
    GameScene();

protected:
    void draw(const HDC& hdc) const;

public:
    //void show(const HDC& hdc) const;  // ī�޶� ��忡 ���� �ٸ���

public:
    void update(const POINT& point);
    void setUp();

    void randomGenFeed();
};