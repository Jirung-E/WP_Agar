#pragma once

#include "Scene.h"


class MainScene : public Scene {
public:
    MainScene();

protected:
    void draw(const HDC& hdc) const;
};