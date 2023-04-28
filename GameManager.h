#pragma once

#include "Scene/GameScene.h"
#include "Scene/MainScene.h"


// 장면전환, 버튼이 눌렸는지 등 관리
class GameManager {
private:
    MainScene main_scene;
    GameScene game_scene;
    Scene* current_scene;
    POINT mouse_position;

public:
    GameManager();
    ~GameManager();

public:
    void show(const HDC& hdc) const;
    void syncSize(const HWND& hWnd);

    void clickScene(const HWND& hWnd, const POINT& point);

private:
    void gameStart();
    void fixCursor(const HWND& hWnd);

    ButtonID buttonClicked(const POINT& point) const;
};