#ifndef INPUT_HPP
#define INPUT_HPP

struct Key_state {
    bool isDown;
    bool wasDown;
};

class Input {
    static Key_state keyStates[0xFF];

public:
    static bool IsKeyDown(int key);
    static bool IsKeyPressed(int key);
    static bool IsKeyReleased(int key);

    static void Update();
};

#endif