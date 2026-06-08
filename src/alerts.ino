// =========================================================
// HOME FRONT COMMAND (Pikud HaOref) RED ALERT
// Shares globals and helpers with main.ino (single Arduino
// translation unit). State/constants are declared in main.ino.
// =========================================================
void prchSilence() {
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void triggerRedAlert() {
  prchAlertActive = true;
  prchAlertStartedMs = millis();
  cachePrchAlert = "";
  wakeDisplay();
  hueAlertOn();
}

void dismissRedAlert() {
  if (!prchAlertActive) return;
  prchAlertActive = false;
  prchSilence();
  hueAlertOff();
  if (!sleepDimmed && !sleepOff) setBacklight(BL_FULL);
  pageDirty = true;
}

void updateRedAlertState() {
  if (!prchAlertActive) return;
  if (millis() - prchAlertStartedMs >= PRCH_ALERT_DURATION_MS) {
    dismissRedAlert();
  }
}

void drawRedAlertOverlay(bool force) {
  if (!prchAlertActive) return;

  bool flashOn = (millis() / 400UL) % 2UL == 0;
  // Pulsing siren tone (no-op if no speaker is wired to GPIO26).
  if (flashOn) tone(BUZZER_PIN, 2200); else noTone(BUZZER_PIN);
  setBacklight(flashOn ? BL_FULL : BL_DIM);


  
  String key = String(flashOn ? 1 : 0);
  if (!force && key == cachePrchAlert) return;
  cachePrchAlert = key;

  uint16_t bg = flashOn ? TFT_RED : 0x6000;  // bright red / dark red
  tft.fillRect(0, 0, SCREEN_W, SCREEN_H, bg);
  tft.setTextColor(TFT_WHITE, bg);
  tft.drawCentreString("! ALERT !", SCREEN_W / 2, 54, 4);
  tft.drawCentreString("TAKE SHELTER", SCREEN_W / 2, 116, 2);

  String label = prchAreaLabel.length() ? prchAreaLabel : "Your area";
  tft.drawCentreString(label.c_str(), SCREEN_W / 2, 158, 2);

  tft.drawCentreString("Tap to dismiss", SCREEN_W / 2, 232, 1);
  tft.setTextColor(0xFFDB, bg);
  tft.drawCentreString("Unofficial - not a substitute", SCREEN_W / 2, 286, 1);
  tft.drawCentreString("for Home Front Command", SCREEN_W / 2, 300, 1);
}

void pollOrefAlert() {
  if (!prchEnabled) return;
  if (prchAlertActive) return;
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - prchLastPollMs < PRCH_POLL_MS) return;
  prchLastPollMs = millis();

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setConnectTimeout(4000);
  http.setTimeout(4000);
  if (!http.begin(client, OREF_ALERTS_URL)) return;
  http.addHeader("Referer", "https://www.oref.org.il/");
  http.addHeader("X-Requested-With", "XMLHttpRequest");
  http.addHeader("Accept", "application/json, text/plain, */*");
  http.setUserAgent("Mozilla/5.0 (Deskbuddy ESP32)");

  int code = http.GET();
  if (code != 200) { http.end(); return; }

  String body = http.getString();
  http.end();

  // Empty body (possibly with a BOM/whitespace) means no active alert.
  int brace = body.indexOf('{');
  if (brace < 0) return;
  body = body.substring(brace);

  JsonDocument doc;  // heap-backed; nationwide alerts can list many areas
  if (deserializeJson(doc, body)) return;

  JsonArray data = doc["data"].as<JsonArray>();
  if (data.isNull()) return;

  bool matched = false;
  for (JsonVariant v : data) {
    const char* area = v.as<const char*>();
    if (!area) continue;
    if (prchAreaMatch.length() == 0) { matched = true; break; }  // empty = all Israel
    if (String(area).indexOf(prchAreaMatch) >= 0) { matched = true; break; }
  }
  if (!matched) return;

  String idStr = doc["id"].as<String>();
  if (idStr.length() && idStr == prchLastAlertId) return;  // already shown this one
  prchLastAlertId = idStr;
  triggerRedAlert();
}
