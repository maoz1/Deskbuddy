// =========================================================
// WEB SERVER / UI  (LittleFS static files + JSON API)
// The HTML/CSS/JS live in /data and are served from flash.
// This file only exposes config as JSON and applies changes.
// =========================================================

// ----- GET /api/config : current settings + option lists -----
void handleApiConfig() {
  JsonDocument d;
  d["ip"]       = WiFi.localIP().toString();
  d["notes"]    = notesText;
  d["nickname"] = buddyNickname;
  d["accent"]   = prefs.getString("accent", "cyan");
  d["text"]     = prefs.getString("text", "standard");
  d["bg"]       = prefs.getString("bg", "slate");
  d["sleepMin"] = sleepIntervalMin;
  d["units"]    = unitKey;
  d["region"]   = regionFormatKey;
  d["tz"]       = timezoneKey;
  d["flashMode"] = flashModeEnabled;
  d["locname"]  = locationName;
  d["lat"]      = LAT;
  d["lng"]      = LNG;
  d["prchEn"]   = prchEnabled;
  d["prchArea"] = prchAreaMatch;
  d["prchLabel"] = prchAreaLabel;
  d["hueEn"]    = hueEnabled;
  d["hueBridge"] = hueBridge;
  d["huePaired"] = (prefs.getString("hueUser", "").length() > 0);
  d["hueStatus"] = hueStatusMsg;
  hueStatusMsg = "";

  JsonArray tm = d["timers"].to<JsonArray>();
  for (int i = 0; i < 6; i++) tm.add(timerPresetMin[i]);

  JsonArray hs = d["homeSlots"].to<JsonArray>();
  JsonArray sl = d["slotLabels"].to<JsonArray>();
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    hs.add(homeWidgetKey(homeWidgetSlots[i]));
    sl.add(homeSlotLabel(i));
  }

  JsonObject opt = d["opt"].to<JsonObject>();

  const char* swKeys[] = {"standard","ice","white","cyan","mint","green","blue","purple","pink","orange","amber","red"};
  JsonArray ac = opt["accent"].to<JsonArray>();
  for (int i = 0; i < 12; i++) { JsonObject o = ac.add<JsonObject>(); o["k"] = swKeys[i]; o["css"] = accentPreviewCss(swKeys[i]); }

  const char* bgKeys[] = {"slate","deep","nordic","forest","coffee","soft","midnight","graphite","garnet","ochre"};
  JsonArray bgo = opt["bg"].to<JsonArray>();
  for (int i = 0; i < 10; i++) { JsonObject o = bgo.add<JsonObject>(); o["k"] = bgKeys[i]; o["css"] = themePreviewCss(bgKeys[i]); }

  // timezone groups (mirrors appendTimezoneOptions)
  struct TZG { const char* label; const char* keys[8]; int count; };
  const TZG groups[] = {
    {"Global", {"utc"}, 1},
    {"Europe", {"atlantic_azores", "europe_west", "uk", "europe_central", "europe_east"}, 5},
    {"Africa & Middle East", {"africa_south", "israel", "middle_east_gulf"}, 3},
    {"Asia", {"india", "thailand", "china", "asia_tokyo", "korea"}, 5},
    {"North America", {"us_eastern", "us_central", "us_mountain", "us_arizona", "us_pacific", "alaska", "hawaii", "canada_atlantic"}, 8},
    {"South America", {"brazil_east", "argentina"}, 2},
    {"Australia & Oceania", {"australia_perth", "australia_darwin", "australia_sydney", "new_zealand"}, 4}
  };
  JsonArray tz = opt["tz"].to<JsonArray>();
  for (const TZG& g : groups)
    for (int i = 0; i < g.count; i++) {
      JsonObject o = tz.add<JsonObject>();
      o["g"] = g.label; o["k"] = g.keys[i]; o["l"] = timezoneLabelByKey(g.keys[i]);
    }

  const HomeWidgetType wtypes[] = {HOME_WIDGET_WEEK, HOME_WIDGET_TIMER, HOME_WIDGET_RAIN, HOME_WIDGET_OUTDOOR,
                                   HOME_WIDGET_KP, HOME_WIDGET_UV, HOME_WIDGET_WIND, HOME_WIDGET_SUN};
  JsonArray wg = opt["widgets"].to<JsonArray>();
  for (HomeWidgetType t : wtypes) { JsonObject o = wg.add<JsonObject>(); o["k"] = homeWidgetKey(t); o["l"] = homeWidgetLabel(t); }

  String out;
  serializeJson(d, out);
  server.send(200, "application/json", out);
}

// ----- POST /api/save : apply a JSON body of settings -----
void handleApiSave() {
  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "application/json", "{\"ok\":false,\"err\":\"bad json\"}");
    return;
  }

  String newAccent = doc["accent"] | "cyan";
  String newBg     = doc["bg"] | "slate";
  String newText   = doc["text"] | "standard";
  String newNotes  = doc["notes"] | notesText;
  String newUnits  = doc["units"] | "metric";
  String newRegion = doc["region"] | "europe";
  String newTz     = sanitizeTimezoneKey(doc["tz"] | timezoneKey);
  String newLoc    = doc["locname"] | locationName;
  String newNick   = doc["nickname"] | buddyNickname;
  float newLat     = doc["lat"] | LAT;
  float newLng     = doc["lng"] | LNG;

  newNotes.trim(); newLoc.trim(); newNick.trim();
  if (newNotes.length() == 0) newNotes = "No notes yet.";
  if (newNotes.length() > 700) newNotes = newNotes.substring(0, 700);
  if (newLoc.length() == 0) newLoc = "Unknown";
  if (newNick.length() > 24) newNick = newNick.substring(0, 24);
  if (newUnits != "metric" && newUnits != "imperial") newUnits = "metric";
  if (newRegion != "europe" && newRegion != "us") newRegion = "europe";

  sleepIntervalMin = constrain((int)(doc["sleepMin"] | sleepIntervalMin), 0, 120);
  flashModeEnabled = doc["flashMode"] | false;

  prchEnabled   = doc["prchEn"] | false;
  prchAreaMatch = doc["prchArea"] | prchAreaMatch;
  prchAreaLabel = doc["prchLabel"] | prchAreaLabel;
  prchAreaMatch.trim(); prchAreaLabel.trim();
  if (prchAreaLabel.length() > 40) prchAreaLabel = prchAreaLabel.substring(0, 40);

  hueEnabled = doc["hueEn"] | false;
  hueBridge  = doc["hueBridge"] | hueBridge;
  hueBridge.trim();

  bool locationChanged = (fabsf(newLat - LAT) > 0.0001f) || (fabsf(newLng - LNG) > 0.0001f) || (newLoc != locationName);

  notesText = newNotes; buddyNickname = newNick; locationName = newLoc;
  LAT = newLat; LNG = newLng;
  unitKey = newUnits; regionFormatKey = newRegion; timezoneKey = newTz;

  JsonArray hs = doc["homeSlots"].as<JsonArray>();
  if (!hs.isNull())
    for (int i = 0; i < HOME_SLOT_COUNT && i < (int)hs.size(); i++)
      homeWidgetSlots[i] = homeWidgetFromKey(hs[i].as<String>());

  JsonArray tm = doc["timers"].as<JsonArray>();
  if (!tm.isNull())
    for (int i = 0; i < 6 && i < (int)tm.size(); i++)
      timerPresetMin[i] = sanitizeTimerMinutes(tm[i] | timerPresetMin[i]);

  prefs.putString("notes", notesText);
  prefs.putString("accent", newAccent);
  prefs.putString("bg", newBg);
  prefs.putString("text", newText);
  prefs.putString("units", unitKey);
  prefs.putString("region", regionFormatKey);
  prefs.putString("tz", timezoneKey);
  prefs.putString("nickname", buddyNickname);
  prefs.putString("locname", locationName);
  prefs.putFloat("lat", LAT);
  prefs.putFloat("lng", LNG);
  prefs.putInt("sleepMin", sleepIntervalMin);
  prefs.putBool("flashMode", flashModeEnabled);
  prefs.putBool("prchEn", prchEnabled);
  prefs.putString("prchArea", prchAreaMatch);
  prefs.putString("prchLabel", prchAreaLabel);
  prefs.putBool("hueEn", hueEnabled);
  prefs.putString("hueBridge", hueBridge);
  for (int i = 0; i < HOME_SLOT_COUNT; i++)
    prefs.putString((String("homeSlot") + String(i)).c_str(), homeWidgetKey(homeWidgetSlots[i]));
  for (int i = 0; i < 6; i++)
    prefs.putInt((String("timer") + String(i)).c_str(), timerPresetMin[i]);

  applyThemeByKey(newAccent, newBg);
  applyTextColorByKey(newText);
  applyDeviceTimezoneByKey(timezoneKey);
  if (!sleepDimmed && !sleepOff) setBacklight(BL_FULL);

  notesDirty = true; pageDirty = true; dataDirty = true;
  cacheClock = ""; cacheHomeEmpty1 = ""; cacheHomeEmpty2 = "";
  cacheFocusTimer = ""; cacheTimerMenu = ""; cacheTimerDone = "";
  for (int i = 0; i < HOME_SLOT_COUNT; i++) cacheHomeSlots[i] = "";
  lastTempText = ""; lastRainText = ""; lastKpText = ""; lastKpLevelText = "";
  lastWindText = ""; lastWindDirText = ""; lastNextSunLabel = ""; lastNextSunTime = ""; lastUptimeText = "";
  if (locationChanged) resetDataCaches();

  server.send(200, "application/json", "{\"ok\":true}");
}

// ----- Hue / alert actions return JSON status -----
void sendActionResult(const String& status) {
  JsonDocument d;
  d["status"] = status;
  d["paired"] = (prefs.getString("hueUser", "").length() > 0);
  d["bridge"] = hueBridge;
  String out; serializeJson(d, out);
  server.send(200, "application/json", out);
}

void handleApiTestAlert() {
  prchLastAlertId = "test-" + String(millis());
  triggerRedAlert();
  sendActionResult("Alert triggered");
}
void handleApiHueFind() { sendActionResult(hueFindBridge()); }
void handleApiHuePair() { sendActionResult(huePair()); }
void handleApiHueTest() { sendActionResult(hueTest()); }

void setupWebServer() {
  LittleFS.begin(true);

  server.on("/api/config", HTTP_GET, handleApiConfig);
  server.on("/api/save", HTTP_POST, handleApiSave);
  server.on("/api/testalert", HTTP_POST, handleApiTestAlert);
  server.on("/api/huefind", HTTP_POST, handleApiHueFind);
  server.on("/api/huepair", HTTP_POST, handleApiHuePair);
  server.on("/api/huetest", HTTP_POST, handleApiHueTest);

  server.serveStatic("/style.css", LittleFS, "/style.css");
  server.serveStatic("/app.js", LittleFS, "/app.js");
  server.serveStatic("/", LittleFS, "/index.html");

  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
}
