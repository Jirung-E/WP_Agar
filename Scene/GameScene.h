#pragma once

#include "Scene.h"
#include "../Map/Map.h"
#include "../Cell/Cell.h"
#include "../Cell/Feed/Feed.h"
#include "../Cell/EnemyCell/EnemyCell.h"

#include <list>


class GameScene : public Scene {
private:
    Map map;
    Cell player;
    std::list<EnemyCell*> enemies;
    std::list<Feed*> feeds;

public:
    GameScene();

protected:
    void draw(const HDC& hdc) const;

public:
    //void show(const HDC& hdc) const;  // 카메라 모드에 따라 다르게

public:
    void setUp();
    void update(const POINT& point);

private:
    void updatePlayer(const POINT& point);
    void updateEnemy();
    void collisionCheck();
    void playerCollisionCheck();
    void enemyCollisionCheck();

public:
    void randomGenFeed();
    void randomGenEnemy();
    
    //void enemyRandomMove();
};