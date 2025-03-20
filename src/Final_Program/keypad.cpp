#include <Keypad.h>
// #include "keypad.h"

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
};
byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {A5, A4, A3, A2};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char getKeyInput() {
    return keypad.getKey();
}
