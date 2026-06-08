// =========================================================
// PAGES / NAVIGATION (moved from main.ino - shares the single Arduino TU)
// =========================================================

// =========================================================
// PAGES
// =========================================================
void drawHomePageFull() {
  tft.fillScreen(COL_BG);
  drawTopBar(homeTitleText());
  drawNavBar();

  cacheClock = "";
  cacheHomeEmpty1 = "";
  cacheHomeEmpty2 = "";
  cacheFocusTimer = "";
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    cacheHomeSlots[i] = "";
  }

  pageDirty = false;
  lastDrawnPage = PAGE_HOME;

  drawClockCardSprite(true);
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    drawHomeSlotWidget(i, true);
  }
  if (focusMenuOpen) drawFocusMenuOverlay(true);
  if (timerDoneDialogOpen) drawTimerDoneOverlay(true);
}

void updateHomeDynamic() {
  if (timerDoneDialogOpen) {
    drawTimerDoneOverlay(false);
    return;
  }

  if (focusMenuOpen) {
    drawFocusMenuOverlay(false);
    return;
  }

  drawClockCardSprite(false);
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    drawHomeSlotWidget(i, false);
  }
}

void drawWeatherPageFull() {
  tft.fillScreen(COL_BG);
  drawTopBar("Weather");
  drawNavBar();

  drawCard(8, PAGE_ROW1_Y, 108, PAGE_WIDGET_H, true);
  drawCard(124, PAGE_ROW1_Y, 108, PAGE_WIDGET_H, true);
  drawCard(8, PAGE_ROW2_Y, 108, PAGE_WIDGET_H, true);
  drawCard(124, PAGE_ROW2_Y, 108, PAGE_WIDGET_H, true);
  drawCard(8, PAGE_ROW3_Y, 108, PAGE_WIDGET_H, true);
  drawCard(124, PAGE_ROW3_Y, 108, PAGE_WIDGET_H, true);

  pageDirty = false;
  lastDrawnPage = PAGE_WEATHER;

  lastTempText = "";
  lastRainText = "";
  lastUvText = "";
  lastUvLevelText = "";
  lastKpText = "";
  lastKpLevelText = "";
  lastWindText = "";
  lastWindDirText = "";
  lastNextSunLabel = "";
  lastNextSunTime = "";
}

void updateWeatherDynamic() {
  String t = tempText();
  String tr = tempRangeText();
  String tempCombined = t + "|" + tr;
  if (tempCombined != lastTempText) {
    tft.fillRect(18, PAGE_ROW1_Y + 30, 88, 30, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Outdoor", 18, PAGE_ROW1_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(t, 18, PAGE_ROW1_Y + 30, 4);
    tft.setTextColor(COL_ACCENT, COL_PANEL);
    tft.drawString(tr, 18, PAGE_ROW1_Y + 54, 1);
    lastTempText = tempCombined;
  }

  String r = rainText();
  if (r != lastRainText) {
    tft.fillRect(134, PAGE_ROW1_Y + 30, 88, 24, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Rain", 134, PAGE_ROW1_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(r, 134, PAGE_ROW1_Y + 30, 4);
    lastRainText = r;
  }

  String u = uvText();
  String ul = uvLevelText();
  if (u != lastUvText || ul != lastUvLevelText || dataDirty) {
    tft.fillRect(18, PAGE_ROW2_Y + 30, 88, 30, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("UV index", 18, PAGE_ROW2_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(u, 18, PAGE_ROW2_Y + 28, 4);
    tft.setTextColor(COL_ACCENT, COL_PANEL);
    tft.drawString(ul, 18, PAGE_ROW2_Y + 52, 1);
    lastUvText = u;
    lastUvLevelText = ul;
  }

  String w = windText();
  String wd = windDirectionText();
  if (w != lastWindText || wd != lastWindDirText) {
    tft.fillRect(134, PAGE_ROW2_Y + 30, 88, 30, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Wind", 134, PAGE_ROW2_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(w, 134, PAGE_ROW2_Y + 28, 4);
    tft.setTextColor(COL_ACCENT, COL_PANEL);
    tft.drawString(wd, 134, PAGE_ROW2_Y + 52, 1);
    lastWindText = w;
    lastWindDirText = wd;
  }

  String nl = nextSunLabel();
  String nt = nextSunTimeText();
  if (nl != lastNextSunLabel || nt != lastNextSunTime) {
    tft.fillRect(18, PAGE_ROW3_Y + 24, 88, 30, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString(nl, 18, PAGE_ROW3_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    if (useUsRegionFormat()) {
      int splitAt = nt.lastIndexOf(' ');
      String sunMain = splitAt > 0 ? nt.substring(0, splitAt) : nt;
      String sunSuffix = splitAt > 0 ? nt.substring(splitAt + 1) : "";
      tft.drawString(sunMain, 18, PAGE_ROW3_Y + 26, 4);
      if (sunSuffix.length() > 0) {
        int suffixX = 18 + tft.textWidth(sunMain, 4) + 3;
        tft.drawString(sunSuffix, suffixX, PAGE_ROW3_Y + 31, 2);
      }
    } else {
      tft.drawString(nt, 18, PAGE_ROW3_Y + 26, 4);
    }
    lastNextSunLabel = nl;
    lastNextSunTime = nt;
  }

  String k = kpText();
  String kl = kpLevelText();
  if (k != lastKpText || kl != lastKpLevelText || dataDirty) {
    tft.fillRect(134, PAGE_ROW3_Y + 30, 88, 30, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("KP index", 134, PAGE_ROW3_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(k, 134, PAGE_ROW3_Y + 28, 4);
    tft.setTextColor(COL_ACCENT, COL_PANEL);
    tft.drawString(kl, 134, PAGE_ROW3_Y + 52, 1);
    lastKpText = k;
    lastKpLevelText = kl;
  }
}

void drawNotesPageFull() {
  tft.fillScreen(COL_BG);
  drawTopBar("Notes");
  drawNavBar();

  drawCard(8, 42, 224, 226, true);

  pageDirty = false;
  lastDrawnPage = PAGE_NOTES;
  lastNotesText = "";
}

void updateNotesDynamic() {
  if (notesText != lastNotesText || notesDirty) {
    tft.fillRect(18, 54, 204, 196, COL_PANEL);
    drawWrappedTextLimited(18, 54, 198, notesText, 2, COL_TEXT, COL_PANEL, 12);
    lastNotesText = notesText;
    notesDirty = false;
  }
}

void drawStatusPageFull() {
  tft.fillScreen(COL_BG);
  drawTopBar("Status");
  drawNavBar();

  drawCard(8, PAGE_ROW1_Y, 108, PAGE_WIDGET_H, true);
  drawCard(124, PAGE_ROW1_Y, 108, PAGE_WIDGET_H, true);
  drawCard(8, PAGE_ROW2_Y, 224, PAGE_WIDGET_H, true);
  drawCard(8, PAGE_ROW3_Y, 108, PAGE_WIDGET_H, true);
  drawCard(124, PAGE_ROW3_Y, 108, PAGE_WIDGET_H, true);

  pageDirty = false;
  lastDrawnPage = PAGE_STATUS;

  lastWifiText = "";
  lastSignalText = "";
  lastIpText = "";
  lastUptimeText = "";
  lastNetworkToggleText = "";
}

void updateStatusDynamic() {
  String w = wifiStatusText();
  if (w != lastWifiText) {
    tft.fillRect(18, PAGE_ROW1_Y + 24, 88, 30, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("WiFi", 18, PAGE_ROW1_Y + 8, 2);
    tft.setTextColor(statusColor(), COL_PANEL);
    tft.drawString(w, 18, PAGE_ROW1_Y + 32, 2);
    lastWifiText = w;
  }

  String s = signalText();
  if (s != lastSignalText) {
    tft.fillRect(134, PAGE_ROW1_Y + 30, 88, 24, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Signal", 134, PAGE_ROW1_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(s, 134, PAGE_ROW1_Y + 30, 4);
    lastSignalText = s;
  }

  String ip = ipText();
  if (ip != lastIpText) {
    tft.fillRect(18, PAGE_ROW2_Y + 30, 200, 18, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("IP address", 18, PAGE_ROW2_Y + 8, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(ip, 18, PAGE_ROW2_Y + 30, 2);
    lastIpText = ip;
  }

  String up = uptimeText();
  String upCombined = up + "|" + lastSyncText();
  if (upCombined != lastUptimeText) {
    tft.fillRect(18, PAGE_ROW3_Y + 26, 88, 26, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Uptime", 18, PAGE_ROW3_Y + 10, 2);
    tft.setTextColor(COL_TEXT, COL_PANEL);
    tft.drawString(up, 18, PAGE_ROW3_Y + 26, 2);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString(lastSyncText(), 18, PAGE_ROW3_Y + 42, 1);
    lastUptimeText = upCombined;
  }

  String networkLabel = wifiEnabled ? "Enabled" : "Disabled";
  if (networkLabel != lastNetworkToggleText) {
    uint16_t btnBg = wifiEnabled ? COL_ACCENT : COL_PANEL_ALT;
    uint16_t btnFg = wifiEnabled ? TFT_BLACK : COL_TEXT;
    uint16_t btnStroke = wifiEnabled ? COL_ACCENT : COL_STROKE;

    tft.fillRect(132, PAGE_ROW3_Y + 8, 92, 42, COL_PANEL);
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Network", 134, PAGE_ROW3_Y + 10, 2);
    tft.fillRoundRect(134, PAGE_ROW3_Y + 30, 88, 22, 8, btnBg);
    tft.drawRoundRect(134, PAGE_ROW3_Y + 30, 88, 22, 8, btnStroke);
    tft.setTextColor(btnFg, btnBg);
    tft.drawCentreString(networkLabel.c_str(), 178, PAGE_ROW3_Y + 32, 2);
    lastNetworkToggleText = networkLabel;
  }
}

void drawCurrentPageFull() {
  // If a conditional page is showing but no longer available, fall back.
  if (currentPage == PAGE_HUE && !hueReady()) currentPage = PAGE_HOME;
  if (currentPage == PAGE_DEVICES && !(bambuEnabled || hpEnabled)) currentPage = PAGE_HOME;

  switch (currentPage) {
    case PAGE_HOME:    drawHomePageFull(); break;
    case PAGE_WEATHER: drawWeatherPageFull(); break;
    case PAGE_NOTES:   drawNotesPageFull(); break;
    case PAGE_STATUS:  drawStatusPageFull(); break;
    case PAGE_HUE:     drawHuePageFull(); break;
    case PAGE_DEVICES: drawDevicesPageFull(); break;
  }

  if (focusMenuOpen && currentPage == PAGE_HOME) drawFocusMenuOverlay(true);
  if (timerDoneDialogOpen) drawTimerDoneOverlay(true);
}

void updateCurrentPageDynamic() {
  if (timerDoneDialogOpen) {
    drawTimerDoneOverlay(false);
    return;
  }

  if (focusMenuOpen && currentPage == PAGE_HOME) {
    drawFocusMenuOverlay(false);
    return;
  }

  switch (currentPage) {
    case PAGE_HOME:    updateHomeDynamic(); break;
    case PAGE_WEATHER: updateWeatherDynamic(); break;
    case PAGE_NOTES:   updateNotesDynamic(); break;
    case PAGE_STATUS:  updateStatusDynamic(); break;
    case PAGE_HUE:     updateHueDynamic(); break;
    case PAGE_DEVICES: updateDevicesDynamic(); break;
  }
}

bool handleFocusMenuTouch(int x, int y) {
  if (!focusMenuOpen) return false;

  if (x < TIMER_MENU_X || x >= TIMER_MENU_X + TIMER_MENU_W || y < TIMER_MENU_Y || y >= TIMER_MENU_Y + TIMER_MENU_H) {
    focusMenuOpen = false;
    cacheTimerMenu = "";
    pageDirty = true;
    return true;
  }

  struct ButtonHit {
    int x;
    int y;
    int w;
    int h;
    int minutes;
  };

  const ButtonHit buttons[] = {
    {34, 124, 74, 28, timerPresetMin[0]},
    {132, 124, 74, 28, timerPresetMin[1]},
    {34, 158, 74, 28, timerPresetMin[2]},
    {132, 158, 74, 28, timerPresetMin[3]},
    {34, 192, 74, 28, timerPresetMin[4]},
    {132, 192, 74, 28, timerPresetMin[5]}
  };

  for (const ButtonHit& btn : buttons) {
    if (x >= btn.x && x < btn.x + btn.w && y >= btn.y && y < btn.y + btn.h) {
      startFocusTimer(btn.minutes);
      pageDirty = true;
      return true;
    }
  }

  if ((focusTimerRunning || focusTimerFinished) && x >= 44 && x < 196 && y >= 224 && y < 250) {
    if (focusTimerRunning || focusTimerFinished) {
      resetFocusTimer();
    } else {
      focusMenuOpen = false;
      cacheTimerMenu = "";
    }
    pageDirty = true;
    return true;
  }

  return true;
}

bool handleHomeTouch(int x, int y) {
  if (currentPage != PAGE_HOME) return false;

  if (focusMenuOpen) return handleFocusMenuTouch(x, y);

  for (int slot = 0; slot < HOME_SLOT_COUNT; slot++) {
    if (homeWidgetSlots[slot] != HOME_WIDGET_TIMER) continue;

    int slotX, slotY, slotW, slotH;
    getHomeSlotRect(slot, slotX, slotY, slotW, slotH);

    if (x >= slotX && x < slotX + slotW && y >= slotY && y < slotY + slotH) {
      if (focusTimerFinished) {
        resetFocusTimer();
      } else {
        focusMenuOpen = true;
        cacheFocusTimer = "";
        clearHomeSlotCaches();
        cacheTimerMenu = "";
      }
      pageDirty = true;
      return true;
    }
  }

  return false;
}

bool handleTimerDoneDialogTouch(int x, int y) {
  (void)x;
  (void)y;
  if (!timerDoneDialogOpen) return false;
  dismissTimerDoneDialog();
  return true;
}

bool handleStatusTouch(int x, int y) {
  if (currentPage != PAGE_STATUS) return false;

  if (x >= 124 && x < 232 && y >= 198 && y < 268) {
    setWifiEnabled(!wifiEnabled);
    return true;
  }

  return false;
}

// =========================================================
// NAVIGATION
// =========================================================
// Nav pages are dynamic: the Hue tab only appears once the bridge is paired.
int buildNavPages(Page* out) {
  int n = 0;
  out[n++] = PAGE_HOME;
  out[n++] = PAGE_WEATHER;
  out[n++] = PAGE_NOTES;
  out[n++] = PAGE_STATUS;
  if (hueReady()) out[n++] = PAGE_HUE;
  if (bambuEnabled || hpEnabled) out[n++] = PAGE_DEVICES;
  return n;
}

const char* pageName(Page p) {
  switch (p) {
    case PAGE_HOME:    return "Home";
    case PAGE_WEATHER: return "Weather";
    case PAGE_NOTES:   return "Notes";
    case PAGE_STATUS:  return "Status";
    case PAGE_HUE:     return "Hue";
    case PAGE_DEVICES: return "Devices";
  }
  return "";
}

void handleNavTouch(int x, int y) {
  if (y < SCREEN_H - NAV_H) return;

  Page pages[5];
  int count = buildNavPages(pages);
  int btnW = SCREEN_W / count;
  int idx = x / btnW;
  if (idx < 0 || idx >= count) return;

  Page newPage = pages[idx];
  if (newPage != currentPage) {
    currentPage = newPage;
    pageDirty = true;
  }
}


// =========================================================
// HUE CONTROL PAGE (all lights = group 0)
// =========================================================
static const int HUE_TOG_X = 8, HUE_TOG_Y = 46, HUE_TOG_W = 224, HUE_TOG_H = 58;
static const int HUE_MIN_X = 8, HUE_PB_Y = 134, HUE_BTN_W = 60, HUE_BTN_H = 52;
static const int HUE_PLUS_X = 172;
static const int HUE_BAR_X = 76, HUE_BAR_W = 88;
static const int HUE_PRE_Y = 218, HUE_PRE_H = 48;

void drawHueToggle() {
  uint16_t bg = huePgOn ? COL_ACCENT : COL_PANEL;
  uint16_t fg = huePgOn ? TFT_BLACK : COL_DIM;
  tft.fillRoundRect(HUE_TOG_X, HUE_TOG_Y, HUE_TOG_W, HUE_TOG_H, 12, bg);
  tft.drawRoundRect(HUE_TOG_X, HUE_TOG_Y, HUE_TOG_W, HUE_TOG_H, 12, huePgOn ? COL_ACCENT : COL_STROKE);
  tft.fillCircle(HUE_TOG_X + 30, HUE_TOG_Y + HUE_TOG_H / 2, 11, huePgOn ? TFT_BLACK : COL_STROKE);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(fg, bg);
  tft.drawString(huePgOn ? "ALL LIGHTS ON" : "ALL LIGHTS OFF", HUE_TOG_X + HUE_TOG_W / 2 + 12, HUE_TOG_Y + HUE_TOG_H / 2, 2);
  tft.setTextDatum(TL_DATUM);
}

void drawHueBri() {
  tft.fillRoundRect(HUE_MIN_X, HUE_PB_Y, HUE_BTN_W, HUE_BTN_H, 10, COL_PANEL);
  tft.drawRoundRect(HUE_MIN_X, HUE_PB_Y, HUE_BTN_W, HUE_BTN_H, 10, COL_STROKE);
  tft.fillRoundRect(HUE_PLUS_X, HUE_PB_Y, HUE_BTN_W, HUE_BTN_H, 10, COL_PANEL);
  tft.drawRoundRect(HUE_PLUS_X, HUE_PB_Y, HUE_BTN_W, HUE_BTN_H, 10, COL_STROKE);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT, COL_PANEL);
  tft.drawString("-", HUE_MIN_X + HUE_BTN_W / 2, HUE_PB_Y + HUE_BTN_H / 2, 4);
  tft.drawString("+", HUE_PLUS_X + HUE_BTN_W / 2, HUE_PB_Y + HUE_BTN_H / 2, 4);

  int pct = (huePgBri * 100) / 254;
  tft.fillRect(HUE_BAR_X, HUE_PB_Y, HUE_BAR_W, HUE_BTN_H, COL_BG);
  tft.setTextColor(huePgOn ? COL_TEXT : COL_DIM, COL_BG);
  tft.drawString(String(pct) + "%", HUE_BAR_X + HUE_BAR_W / 2, HUE_PB_Y + 12, 2);
  int trackY = HUE_PB_Y + 30, trackH = 12;
  tft.fillRoundRect(HUE_BAR_X, trackY, HUE_BAR_W, trackH, 6, COL_PANEL);
  int fillW = (HUE_BAR_W * pct) / 100;
  if (fillW > 4) tft.fillRoundRect(HUE_BAR_X, trackY, fillW, trackH, 6, huePgOn ? COL_ACCENT : COL_STROKE);
  tft.setTextDatum(TL_DATUM);
}

void drawHuePresets() {
  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("Presets", 8, 196, 2);
  const uint16_t cols[5] = {0xFD20, 0xAEDF, COL_RED, COL_GREEN, 0x3D9F}; // warm, cool, red, green, blue
  for (int i = 0; i < 5; i++) {
    int x = 8 + i * 45;
    tft.fillRoundRect(x, HUE_PRE_Y, 42, HUE_PRE_H, 10, cols[i]);
    tft.drawRoundRect(x, HUE_PRE_Y, 42, HUE_PRE_H, 10, COL_STROKE);
  }
}

void drawHuePageFull() {
  tft.fillScreen(COL_BG);
  drawTopBar("Hue Lights");
  drawNavBar();

  bool on; int bri;
  if (hueGetGroup(on, bri)) { huePgOn = on; if (bri > 0) huePgBri = bri; }

  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("Brightness", 8, 114, 2);
  drawHueToggle();
  drawHueBri();
  drawHuePresets();

  pageDirty = false;
  lastDrawnPage = PAGE_HUE;
}

void updateHueDynamic() {
  // No periodic refresh; the screen reflects local state and updates on touch.
}

bool handleHueTouch(int x, int y) {
  if (currentPage != PAGE_HUE) return false;

  if (x >= HUE_TOG_X && x < HUE_TOG_X + HUE_TOG_W && y >= HUE_TOG_Y && y < HUE_TOG_Y + HUE_TOG_H) {
    huePgOn = !huePgOn;
    hueSetGroupOn(huePgOn);
    drawHueToggle();
    drawHueBri();
    return true;
  }
  if (x >= HUE_MIN_X && x < HUE_MIN_X + HUE_BTN_W && y >= HUE_PB_Y && y < HUE_PB_Y + HUE_BTN_H) {
    huePgBri = constrain(huePgBri - 38, 1, 254);
    huePgOn = true;
    hueSetGroupBri(huePgBri);
    drawHueToggle();
    drawHueBri();
    return true;
  }
  if (x >= HUE_PLUS_X && x < HUE_PLUS_X + HUE_BTN_W && y >= HUE_PB_Y && y < HUE_PB_Y + HUE_BTN_H) {
    huePgBri = constrain(huePgBri + 38, 1, 254);
    huePgOn = true;
    hueSetGroupBri(huePgBri);
    drawHueToggle();
    drawHueBri();
    return true;
  }
  if (y >= HUE_PRE_Y && y < HUE_PRE_Y + HUE_PRE_H && x >= 8 && x < 233) {
    int idx = (x - 8) / 45;
    switch (idx) {
      case 0: hueSetGroupCt(454); break;            // warm white
      case 1: hueSetGroupCt(250); break;            // cool white
      case 2: hueSetGroupHueSat(0, 254); break;     // red
      case 3: hueSetGroupHueSat(25500, 254); break; // green
      case 4: hueSetGroupHueSat(46920, 254); break; // blue
      default: return true;
    }
    huePgOn = true;
    drawHueToggle();
    return true;
  }
  return false;
}

// =========================================================
// DEVICES PAGE (Bambu Lab printer + HP SmartTank)
// =========================================================
static String lastBambuCache = "";
static String lastHpCache = "";

static int devBambuY() { return 46; }
static int devHpY() { return bambuEnabled ? (46 + 126) : 46; }

static void drawDeviceCardFrame(int y, int h, const char* title) {
  tft.fillRoundRect(8, y, 224, h, 12, COL_PANEL);
  tft.drawRoundRect(8, y, 224, h, 12, COL_STROKE);
  tft.setTextColor(COL_ACCENT, COL_PANEL);
  tft.drawString(title, 18, y + 8, 2);
}

static void drawBambuValues(bool force) {
  if (!bambuEnabled) return;
  int y = devBambuY();
  String cache = String(bambuOnline) + "|" + bambuState + "|" + String(bambuPct) + "|" +
                 String(bambuRemainMin) + "|" + String((int)bambuNozzle) + "|" + String((int)bambuBed);
  if (!force && cache == lastBambuCache) return;
  lastBambuCache = cache;

  tft.fillRect(12, y + 28, 216, 86, COL_PANEL);
  if (!bambuOnline) {
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Connecting...", 18, y + 44, 2);
    return;
  }

  tft.setTextColor(COL_TEXT, COL_PANEL);
  tft.drawString(bambuState, 18, y + 30, 2);

  int barY = y + 56, barW = 204;
  tft.fillRoundRect(18, barY, barW, 14, 7, COL_PANEL_ALT);
  int fw = (barW * constrain(bambuPct, 0, 100)) / 100;
  if (fw > 4) tft.fillRoundRect(18, barY, fw, 14, 7, COL_ACCENT);

  tft.setTextColor(COL_DIM, COL_PANEL);
  tft.drawString(String(bambuPct) + "%   " + String(bambuRemainMin) + " min left", 18, y + 76, 2);
  tft.drawString("Nozzle " + String((int)bambuNozzle) + "C   Bed " + String((int)bambuBed) + "C", 18, y + 96, 1);
}

static void drawHpValues(bool force) {
  if (!hpEnabled) return;
  int y = devHpY();
  String cache = String(hpOnline) + "|" + hpState + "|" + String(hpInk);
  if (!force && cache == lastHpCache) return;
  lastHpCache = cache;

  tft.fillRect(12, y + 28, 216, 52, COL_PANEL);
  if (!hpOnline) {
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Offline", 18, y + 40, 2);
    return;
  }
  tft.setTextColor(COL_TEXT, COL_PANEL);
  tft.drawString(hpState, 18, y + 32, 2);
  if (hpInk >= 0) {
    tft.setTextColor(COL_DIM, COL_PANEL);
    tft.drawString("Ink ~" + String(hpInk) + "%", 18, y + 56, 2);
  }
}

void drawDevicesPageFull() {
  tft.fillScreen(COL_BG);
  drawTopBar("Devices");
  drawNavBar();

  lastBambuCache = "";
  lastHpCache = "";
  if (bambuEnabled) drawDeviceCardFrame(devBambuY(), 118, "Bambu Lab");
  if (hpEnabled)    drawDeviceCardFrame(devHpY(), 86, "HP SmartTank");
  drawBambuValues(true);
  drawHpValues(true);

  pageDirty = false;
  lastDrawnPage = PAGE_DEVICES;
}

void updateDevicesDynamic() {
  drawBambuValues(false);
  drawHpValues(false);
}
