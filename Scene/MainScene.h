#pragma once

#include "Scene.h"


class MainScene : public Scene {
public:
    MainScene();

public:
    void show(const HDC& hdc);
};