#include "Button.h"


Button::Button(const tstring& text, const Point& position, double width, double height) : text { text }, 
position { position }, width { width }, height { height },
background_color { White }, text_color { Black }, border_color { Black }, border_width { 1 }, id { None } {

}


void Button::show(const HDC& hdc, const RECT& valid_area) const {
    drawBase(hdc, valid_area);
    drawText(hdc, valid_area);
}

RECT Button::absoluteArea(const RECT& valid_area) const {
    RECT area = rectToSquare(valid_area);
    int w = area.right - area.left;
    int h = area.bottom - area.top;
    int x = area.left + w * position.x / 100;
    int y = area.top + h * position.y / 100;
    return RECT { x, y, int(x + w * width / 100), int(y + h * height / 100) };
}


void Button::drawBase(const HDC& hdc, const RECT& valid_area) const {
    HBRUSH bg_br = CreateSolidBrush(background_color);
    HPEN bg_pen = CreatePen(PS_SOLID, border_width, border_color);
    HBRUSH old_br = (HBRUSH)SelectObject(hdc, bg_br);
    HPEN old = (HPEN)SelectObject(hdc, bg_pen);

    RECT rect = absoluteArea(valid_area);
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

    SelectObject(hdc, old_br);
    DeleteObject(bg_pen);
    DeleteObject(bg_br);
}

void Button::drawText(const HDC& hdc, const RECT& valid_area) const {
    LOGFONT logfont;
    GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &logfont);

    RECT rect = absoluteArea(valid_area);
    rect %= 90;
    logfont.lfHeight = (rect.bottom - rect.top) * 90 / 100;

    HFONT hNewFont = CreateFontIndirect(&logfont);
    HFONT old_font = (HFONT)SelectObject(hdc, hNewFont);

    int center_x = (rect.left + rect.right) / 2;
    int center_y = (rect.top + rect.bottom) / 2;
    SIZE size;
    GetTextExtentPoint32W(hdc, text.c_str(), text.length(), &size);
    //TextOut(hdc, center_x - size.cx/2, center_y - size.cy/2, text.c_str(), text.length());
    DrawText(hdc, text.c_str(), text.length(), &rect, DT_EDITCONTROL | DT_WORDBREAK | DT_CENTER);

    SelectObject(hdc, old_font);
    DeleteObject(hNewFont);
}