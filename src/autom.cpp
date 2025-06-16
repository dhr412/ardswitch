#include <windows.h>

int main() {
    INPUT keys[4] = {};

    // Press ALT
    keys[0].type = INPUT_KEYBOARD;
    keys[0].ki.wVk = VK_MENU;

    // Press ESC
    keys[1].type = INPUT_KEYBOARD;
    keys[1].ki.wVk = VK_ESCAPE;

    // Release ESC
    keys[2].type = INPUT_KEYBOARD;
    keys[2].ki.wVk = VK_ESCAPE;
    keys[2].ki.dwFlags = KEYEVENTF_KEYUP;

    // Release ALT
    keys[3].type = INPUT_KEYBOARD;
    keys[3].ki.wVk = VK_MENU;
    keys[3].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT sent = SendInput(4, keys, sizeof(INPUT));

    return (sent != 4) ? 1 : 0;
}
