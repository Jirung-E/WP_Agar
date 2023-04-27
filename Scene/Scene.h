#pragma once

#include <Windows.h>


enum SceneID {
    Main, Game, PlayerCustomize, Game_Setting
};


class Scene {
protected:
    SceneID id;
    RECT valid_area;

//    HDC mdc;                    // 이게 맞나?
//    HBITMAP hbitmap;            // 그릴곳이 있어야 하는데...      -> 그냥 함수 하나에서 hdc받아오면 거기서 그리기 함수 다 호출하는식으로 하자.

public:
    Scene(const SceneID& id);

public:
    virtual void show(const HDC& hdc) const = 0;
    SceneID getID() const;
};