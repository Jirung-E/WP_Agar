#pragma once

#include <Windows.h>


enum SceneID {
    Main, Game, PlayerCustomize, Game_Setting
};


class Scene {
protected:
    SceneID id;
    RECT valid_area;

//    HDC mdc;                    // �̰� �³�?
//    HBITMAP hbitmap;            // �׸����� �־�� �ϴµ�...      -> �׳� �Լ� �ϳ����� hdc�޾ƿ��� �ű⼭ �׸��� �Լ� �� ȣ���ϴ½����� ����.

public:
    Scene(const SceneID& id);

public:
    virtual void show(const HDC& hdc) const = 0;
    SceneID getID() const;
};