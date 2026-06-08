#pragma once
#include <Arduino.h>

// =========================================================
// Philips Hue red-alert integration (local bridge v1 API)
// =========================================================

// Shared state (defined in hue.cpp)
extern bool hueEnabled;
extern String hueBridge;     // bridge IP
extern String hueUser;       // API username/token from pairing
extern String hueStatusMsg;  // feedback shown in the web UI

// Public API
String hueFindBridge();  // auto-discover bridge IP via Hue cloud discovery
String huePair();        // link-button pairing (press bridge button first)
String hueTest();        // flash red ~3s then restore, returns status text
void   hueAlertOn();     // capture current state + turn all lights red
void   hueReflash();     // re-trigger the breathe effect during a long alert
void   hueAlertOff();    // restore the pre-alert light state

// On-device control screen (group 0 = all lights)
bool   hueReady();                       // true once paired (token present)
bool   hueGetGroup(bool& on, int& bri);  // read all-lights state
void   hueSetGroupOn(bool on);
void   hueSetGroupBri(int bri);          // 1..254
void   hueSetGroupCt(int ct);            // colour temperature (153 cool .. 500 warm)
void   hueSetGroupHueSat(int hue, int sat);
