#include <Windows.h>

#include "input.hpp"

Key_state Input::keyStates[0xFF] = {0};

bool Input::IsKeyDown(int key) {
    return keyStates[key].isDown;
}

bool Input::IsKeyPressed(int key) {
    return keyStates[key].isDown && !keyStates[key].wasDown;
}

bool Input::IsKeyReleased(int key) {
    return !keyStates[key].isDown && keyStates[key].wasDown;
}

void Input::Update() {
    for (int i = 0; i < 0xFF; ++i) {
        keyStates[i].wasDown = keyStates[i].isDown;
        keyStates[i].isDown = GetAsyncKeyState(i) & 0x8000;
    }
}