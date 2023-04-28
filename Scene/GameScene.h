#pragma once

#include "Scene.h"


class GameScene : public Scene {
public:
    GameScene();

protected:
    void draw(const HDC& hdc) const;

public:
    void update();
};