#pragma once

#include <Windows.h>


enum SceneID {
    Main, Game, PlayerCustomize, Game_Setting
};


class Scene {
protected:
    SceneID id;
    int horizontal_length;
    int vertical_length;
    RECT valid_area;

public:
    Scene(const SceneID& id);

protected:
    void drawBackground(const HDC& hdc, const COLORREF& color) const;
    virtual void draw(const HDC& hdc) const = 0;

public:
    void show(const HDC& hdc) const;
    void syncSize(const HWND& hWnd);
    SceneID getID() const;
};