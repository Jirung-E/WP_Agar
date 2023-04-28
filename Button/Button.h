#pragma once

#include "../Util/Util.h"
#include "../Engine/Point.h"


enum ButtonID {
    None, StartButton
};


class Button {
public:
    Point position;
    double width;
    double height;
    COLORREF background_color;
    COLORREF text_color;
    COLORREF border_color;
    int border_width;
    tstring text;
    ButtonID id;

public:
    Button(const tstring& text = L"", const Point& position = { 0, 0 }, double width = 1, double height = 1);

public:
    void show(const HDC& hdc, const RECT& valid_area) const;
    RECT absoluteArea(const RECT& valid_area) const;

private:
    void drawBase(const HDC& hdc, const RECT& valid_area) const;
    void drawText(const HDC& hdc, const RECT& valid_area) const;
};