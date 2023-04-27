#include "MainScene.h"

#include "../Util/Util.h"


MainScene::MainScene() : Scene { Main } {

}


void MainScene::draw(const HDC& hdc) const {
    HBRUSH br = CreateSolidBrush(Black);
    HBRUSH old = (HBRUSH)SelectObject(hdc, br);
    Rectangle(hdc, valid_area.left, valid_area.top, valid_area.right, valid_area.bottom);
    SelectObject(hdc, old);
    DeleteObject(br);

    // Draw Start Button


    // Draw Customize Button


    // Draw Quit Button
    
}