#pragma once

#include "Scene.h"
#include "../Map/Map.h"
#include "../Cell/Cell.h"
#include "../Cell/Feed/Feed.h"
#include "../Cell/EnemyCell/EnemyCell.h"
#include "../Cell/Trap/Trap.h"
#include "../Cell/Virus.h"

#include <list>
#include <ctime>


class GameScene : public Scene {
public:
    enum CameraMode {
        Fixed, Dynamic
    };
    bool show_score;

private:
    Map map;
    Virus player;
    std::list<EnemyCell*> enemies;
    std::list<Trap*> traps;
    std::list<Feed*> feeds;
    Button resume_button;
    Button quit_button;
    bool paused;
    TextBox game_over_message;
    bool game_over;

    CameraMode cam_mode;

    double play_time;
    clock_t start_time;
    clock_t end_time;

    int feed_erase_count;

public:
    GameScene();

public:
    void setUp();
    void update(const POINT& point);

    void pause();
    void resume();

private:
    void updatePlayer(const POINT& point);
    void updateEnemy();
    void updateFeeds();
    void updateTraps();
    void collisionCheck();
    void playerCollisionCheck();
    void enemyCollisionCheck();

protected:
    void draw(const HDC& hdc) const;
    RECT getViewArea() const;
    void drawScore(const HDC& hdc) const;
    void drawPauseScene(const HDC& hdc) const;
    void drawGameOverScene(const HDC& hdc) const;

public:
    void setCameraMode(const CameraMode& mode);

    void randomGenFeed();
    void randomGenEnemy();
    void randomGenTrap();

    ButtonID clickL(const POINT& point);
    ButtonID clickR(const POINT& point);
};