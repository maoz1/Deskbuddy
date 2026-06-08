// =========================================================
// CONFIG: theme, settings, timezone (moved from main.ino)
// =========================================================

// =========================================================
// THEME / SETTINGS
// =========================================================
void applyThemeByKey(const String& accentKey, const String& bgKey) {
  if (accentKey == "standard")    COL_ACCENT = 0xEF7D;
  else if (accentKey == "cyan")   COL_ACCENT = 0x5EFA;
  else if (accentKey == "ice")    COL_ACCENT = 0xEFFF;
  else if (accentKey == "white")  COL_ACCENT = TFT_WHITE;
  else if (accentKey == "mint")   COL_ACCENT = 0x07F0;
  else if (accentKey == "green")  COL_ACCENT = TFT_GREEN;
  else if (accentKey == "blue")   COL_ACCENT = 0x3D9F;
  else if (accentKey == "purple") COL_ACCENT = 0xA2F5;
  else if (accentKey == "pink")   COL_ACCENT = 0xF97F;
  else if (accentKey == "orange") COL_ACCENT = 0xFD20;
  else if (accentKey == "amber")  COL_ACCENT = 0xFEA0;
  else if (accentKey == "red")    COL_ACCENT = TFT_RED;
  else                            COL_ACCENT = 0x5EFA;

  if (bgKey == "slate") {
    COL_BG = 0x08A3; COL_PANEL = 0x1106; COL_PANEL_ALT = 0x18C7; COL_STROKE = 0x31EC;
  } else if (bgKey == "deep") {
    COL_BG = 0x0000; COL_PANEL = 0x0841; COL_PANEL_ALT = 0x1082; COL_STROKE = 0x2945;
  } else if (bgKey == "nordic") {
    COL_BG = 0x0864; COL_PANEL = 0x10C6; COL_PANEL_ALT = 0x1908; COL_STROKE = 0x3A2D;
  } else if (bgKey == "forest") {
    COL_BG = 0x0208; COL_PANEL = 0x0ACB; COL_PANEL_ALT = 0x134D; COL_STROKE = 0x2D72;
  } else if (bgKey == "coffee") {
    COL_BG = 0x18A3; COL_PANEL = 0x2945; COL_PANEL_ALT = 0x39C7; COL_STROKE = 0x5A89;
  } else if (bgKey == "soft") {
    COL_BG = 0x10A2; COL_PANEL = 0x1924; COL_PANEL_ALT = 0x2145; COL_STROKE = 0x3A49;
  } else if (bgKey == "midnight") {
    COL_BG = 0x0008; COL_PANEL = 0x0011; COL_PANEL_ALT = 0x0018; COL_STROKE = 0x3A7F;
  } else if (bgKey == "graphite") {
    COL_BG = 0x1082; COL_PANEL = 0x18C3; COL_PANEL_ALT = 0x2104; COL_STROKE = 0x4208;
  } else if (bgKey == "garnet") {
    COL_BG = 0x1004; COL_PANEL = 0x1886; COL_PANEL_ALT = 0x20E8; COL_STROKE = 0x41AC;
  } else if (bgKey == "ochre") {
    COL_BG = 0x20E1; COL_PANEL = 0x3184; COL_PANEL_ALT = 0x4226; COL_STROKE = 0x632B;
  } else {
    COL_BG = 0x08A3; COL_PANEL = 0x1106; COL_PANEL_ALT = 0x18C7; COL_STROKE = 0x31EC;
  }
}

void applyTextColorByKey(const String& key) {
  textColorKey = key;

  if (key == "standard") {
    COL_TEXT = 0xEF7D; COL_DIM  = 0x94B2;
  } else if (key == "white") {
    COL_TEXT = TFT_WHITE; COL_DIM = 0xBDF7;
  } else if (key == "ice") {
    COL_TEXT = 0xEFFF; COL_DIM = 0x9D7F;
  } else if (key == "mint") {
    COL_TEXT = 0x07F0; COL_DIM = 0x05EC;
  } else if (key == "orange") {
    COL_TEXT = 0xFD20; COL_DIM = 0xBA26;
  } else if (key == "amber") {
    COL_TEXT = 0xFEA0; COL_DIM = 0xBCE0;
  } else if (key == "green") {
    COL_TEXT = TFT_GREEN; COL_DIM = 0x86E8;
  } else if (key == "cyan") {
    COL_TEXT = 0x5EFA; COL_DIM = 0x3D96;
  } else if (key == "blue") {
    COL_TEXT = 0x3D9F; COL_DIM = 0x22B1;
  } else if (key == "purple") {
    COL_TEXT = 0xA2F5; COL_DIM = 0x79ED;
  } else if (key == "red") {
    COL_TEXT = TFT_RED; COL_DIM = 0xB9E7;
  } else if (key == "pink") {
    COL_TEXT = 0xF97F; COL_DIM = 0xC2F1;
  } else {
    COL_TEXT = 0xEF7D;
    COL_DIM  = 0x94B2;
    textColorKey = "standard";
  }
}

void loadStoredSettings() {
  prefs.begin("deskbuddy", false);

  String accent = prefs.getString("accent", "cyan");
  String bg     = prefs.getString("bg", "slate");
  String txt    = prefs.getString("text", "standard");

  notesText        = prefs.getString("notes", "No notes yet.");
  buddyNickname    = prefs.getString("nickname", "");
  locationName     = prefs.getString("locname", "Berlin");
  LAT              = prefs.getFloat("lat", 52.5200f);
  LNG              = prefs.getFloat("lng", 13.4050f);
  sleepIntervalMin = prefs.getInt("sleepMin", 10);
  unitKey          = prefs.getString("units", "metric");
  regionFormatKey  = prefs.getString("region", "europe");
  timezoneKey      = sanitizeTimezoneKey(prefs.getString("tz", "europe_central"));
  flashModeEnabled = prefs.getBool("flashMode", false);
  wifiEnabled      = prefs.getBool("wifiEnabled", true);

  prchEnabled      = prefs.getBool("prchEn", false);
  prchAreaMatch    = prefs.getString("prchArea", "");
  prchAreaLabel    = prefs.getString("prchLabel", "");

  hueEnabled       = prefs.getBool("hueEn", false);
  hueBridge        = prefs.getString("hueBridge", "");
  hueUser          = prefs.getString("hueUser", "");

  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    String key = String("homeSlot") + String(i);
    homeWidgetSlots[i] = homeWidgetFromKey(prefs.getString(key.c_str(), homeWidgetKey(homeWidgetSlots[i])));
  }

  for (int i = 0; i < 6; i++) {
    String key = String("timer") + String(i);
    timerPresetMin[i] = sanitizeTimerMinutes(prefs.getInt(key.c_str(), timerPresetMin[i]));
  }

  if (unitKey != "metric" && unitKey != "imperial") unitKey = "metric";
  if (regionFormatKey != "europe" && regionFormatKey != "us") regionFormatKey = "europe";
  buddyNickname.trim();
  applyThemeByKey(accent, bg);
  applyTextColorByKey(txt);
  applyDeviceTimezoneByKey(timezoneKey);
}

void resetDataCaches() {
  tempC = NAN;
  precipMm = NAN;
  windSpeedMs = NAN;
  windDirectionDeg = NAN;
  kpIndex = NAN;
  sunriseMin = -1;
  sunsetMin = -1;
  lastSunYmd = -1;
  lastWeatherFetch = 0;
  lastKpFetch = 0;
  dataDirty = true;
  pageDirty = true;
}

