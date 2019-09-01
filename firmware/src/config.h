#include <pins_arduino.h>

//led strip 0 - white
const int PIN_L0 = D0;

//led strip 1 - RGB
const int PIN_L1_R = D1;
const int PIN_L1_G = D2;
const int PIN_L1_B = D3;

//SD
const int PIN_CS = D8;

const int SERIAL_SPEED = 115200;

const long WAIT_FOR_STATE_SAVE = 5 * 1000;//ms

const char* SETTINGS_FILE = "/SETTIN~1.JSO"; //FAT names...
const char* L0_FILE = "/L0";
const char* L1_FILE = "/L1";
