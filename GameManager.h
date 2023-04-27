#pragma once

#include "Scene/GameScene.h"
#include "Scene/MainScene.h"


// �����ȯ, ��ư�� ���ȴ��� �� ����
class GameManager {
private:
    MainScene main_scene;
    GameScene game_scene;
    Scene* current_scene;

public:
    GameManager();
    ~GameManager();
};