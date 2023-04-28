#pragma once

#include "Scene.h"

#include "../Button/Button.h"


class MainScene : public Scene {
public:
    Button start_button;

public:
    MainScene();

protected:
    void draw(const HDC& hdc) const;

public:
    ButtonID click(const POINT& point) const;
};