#include <pins_arduino.h>

//led strip 0 - white
const int PIN_L0 = D1;

//led strip 1 - RGB
const int PIN_L1_R = D2;
const int PIN_L1_G = D3;
const int PIN_L1_B = D4;

const int ANIMATION_SPEED = 400;

//SD
const int PIN_CS = D8;

const int SERIAL_SPEED = 115200;

const long WAIT_FOR_STATE_SAVE = 5 * 1000;//ms

const char* SETTINGS_FILE = "/SETTIN~1.JSO"; //FAT names...
const char* L0_FILE = "/L0";
const char* L1_FILE = "/L1";
const char* L0_NIGHT_FILE = "/L0.n";
const char* L1_NIGHT_FILE = "/L1.n";
