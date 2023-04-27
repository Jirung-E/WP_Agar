#pragma once

#include "Scene/GameScene.h"
#include "Scene/MainScene.h"


// 장면전환, 버튼이 눌렸는지 등 관리
class GameManager {
private:
    MainScene main_scene;
    GameScene game_scene;
    Scene* current_scene;

public:
    GameManager();
    ~GameManager();
};