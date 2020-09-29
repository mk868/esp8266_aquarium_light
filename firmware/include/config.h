#include <pins_arduino.h>

//led strip 0 - white
const int PIN_L0 = D0;

//led strip 1 - RGB
const int PIN_L1_R = D3;
const int PIN_L1_G = D4;
const int PIN_L1_B = D9;

const int ANIMATION_SPEED = 400;

//SD
const int PIN_CS = D8;

const int SERIAL_SPEED = 115200;

const long WAIT_FOR_STATE_SAVE = 5 * 1000; //ms

//FAT names...
const char *CORE_SETTINGS_FILE = "/SETTIN~1.JSO";
const char *SETTINGS_FILE = "/S";
const char *DAY_L0_FILE = "/L0";
const char *DAY_L1_FILE = "/L1";
const char *NIGHT_L0_FILE = "/L0.n";
const char *NIGHT_L1_FILE = "/L1.n";
