#include "GameManager.h"


GameManager::GameManager() : main_scene { }, game_scene { }, current_scene { &main_scene }, mouse_position { 0, 0 } {

}

GameManager::~GameManager() {

}


void GameManager::show(const HDC& hdc) const {
	current_scene->show(hdc);
}

void GameManager::syncSize(const HWND& hWnd) {
	current_scene->syncSize(hWnd);
	GetCursorPos(&mouse_position);
	ScreenToClient(hWnd, &mouse_position);
}


void GameManager::clickScene(const HWND& hWnd, const POINT& point) {
	switch(buttonClicked(point)) {
	case StartButton:
		fixCursor(hWnd);
		//ShowCursor(false);
		gameStart();
	}
}


void GameManager::gameStart() {
    current_scene = &game_scene;
}

void GameManager::fixCursor(const HWND& hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	POINT lt = { rect.left, rect.top };
	ClientToScreen(hWnd, &lt);
	rect.left += lt.x;
	rect.top += lt.y;
	rect.right += lt.x;
	rect.bottom += lt.y;
	ClipCursor(&rect);
	SetCursorPos((rect.left + rect.right)/2, (rect.top + rect.bottom)/2);
}

ButtonID GameManager::buttonClicked(const POINT& point) const {
	return main_scene.click(point);;
}