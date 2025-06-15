#include <windows.h>

int main() {
    INPUT keys[4] = {};

    const WORD vKeys[] = {VK_MENU, VK_ESCAPE};
    for (int i = 0; i < 2; ++i) {
        // Press key
        keys[i * 2].type = INPUT_KEYBOARD;
        keys[i * 2].ki.wVk = vKeys[i];

        // Release key
        keys[i * 2 + 1].type = INPUT_KEYBOARD;
        keys[i * 2 + 1].ki.wVk = vKeys[i];
        keys[i * 2 + 1].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    UINT sent = SendInput(4, keys, sizeof(INPUT));

    return (sent != 4) ? 1 : 0;
}
