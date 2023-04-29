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
	syncMousePosition(hWnd);
}


void GameManager::keyboardInput(const HWND& hWnd, int keycode) {
	switch(keycode) {
	case VK_ESCAPE:
		quit(hWnd);
		break;
	}
}


void GameManager::clickScene(const HWND& hWnd, const POINT& point) {
	switch(buttonClicked(point)) {
	case StartButton:
		fixCursor(hWnd);
		//ShowCursor(false);
		gameStart(hWnd);
	}
}

void GameManager::update(const HWND& hWnd) {
	// 버튼에 오버라이드 될시 버튼 강조, 게임시작시에는 이동
	//syncSize(hWnd);
	switch(current_scene->getID()) {
	case Game:
		game_scene.update(mouse_position);
		break;
	case Main:

		break;
	}
}

void GameManager::quit(const HWND& hWnd) {
	switch(current_scene->getID()) {
	case Game:
		current_scene = &main_scene;
		KillTimer(hWnd, GenerateFeeds);
		releaseCursor();
		break;
	case Main:
		PostQuitMessage(0);
		break;
	}
}


void GameManager::setTimers(const HWND& hWnd) {
	SetTimer(hWnd, Display, fps(60), NULL);
	SetTimer(hWnd, UpdateGame, fps(60), NULL);
}

void GameManager::timer(const HWND& hWnd, int id) {
	switch(id) {
	case Display:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case UpdateGame:
		update(hWnd);
		break;
	case GenerateFeeds:
		game_scene.randomGenFeed();
		break;
	}
}


void GameManager::gameStart(const HWND& hWnd) {
	game_scene.setUp();
    current_scene = &game_scene;
	SetTimer(hWnd, GenerateFeeds, 10000, NULL);
	game_scene.randomGenFeed();
	game_scene.randomGenFeed();
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

void GameManager::releaseCursor() {
	ClipCursor(NULL);
}

ButtonID GameManager::buttonClicked(const POINT& point) const {
	switch(current_scene->getID()) {
	case SceneID::Main:
		return main_scene.click(point);;
	default:
		return None;
	}
}

void GameManager::syncMousePosition(const HWND& hWnd) {
	GetCursorPos(&mouse_position);
	ScreenToClient(hWnd, &mouse_position);
}