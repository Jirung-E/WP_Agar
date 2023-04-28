#include "GameScene.h"

#include "../Util/Util.h"


GameScene::GameScene() : Scene { Game } {

}


void GameScene::draw(const HDC& hdc) const {
    drawBackground(hdc, Black);

    // Draw Player
    HBRUSH br = CreateSolidBrush(Red);
    HBRUSH old = (HBRUSH)SelectObject(hdc, br);
    Rectangle(hdc, valid_area.left + horizontal_length/3, valid_area.top + vertical_length/3, valid_area.right - horizontal_length/3, valid_area.bottom - vertical_length/3);
    SelectObject(hdc, old);
    DeleteObject(br);

    // Draw Score

}

void GameScene::update() {
    POINT p;
    GetCursorPos(&p);
}