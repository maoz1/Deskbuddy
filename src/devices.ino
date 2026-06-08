// =========================================================
// HOME DEVICES
//   Bambu Lab printer  -> local MQTT (LAN mode, TLS 8883)
//   HP SmartTank       -> local HTTP status XML
// Config/status globals are declared in main.ino.
// =========================================================

static WiFiClientSecure bambuTls;
static PubSubClient bambuMqtt(bambuTls);
static unsigned long bambuLastAttempt = 0;
static unsigned long hpLastPoll = 0;
static const unsigned long HP_POLL_MS = 15000UL;
static const unsigned long BAMBU_RETRY_MS = 6000UL;

// ---- Bambu: parse an MQTT status report ----
void bambuCallback(char* topic, byte* payload, unsigned int len) {
  JsonDocument filter;
  JsonObject fp = filter["print"].to<JsonObject>();
  fp["gcode_state"] = true;
  fp["mc_percent"] = true;
  fp["mc_remaining_time"] = true;
  fp["nozzle_temper"] = true;
  fp["bed_temper"] = true;
  fp["gcode_file"] = true;

  JsonDocument doc;
  if (deserializeJson(doc, payload, len, DeserializationOption::Filter(filter))) return;
  JsonObject p = doc["print"];
  if (p.isNull()) return;

  if (!p["gcode_state"].isNull())       bambuState     = p["gcode_state"].as<String>();
  if (!p["mc_percent"].isNull())        bambuPct       = p["mc_percent"].as<int>();
  if (!p["mc_remaining_time"].isNull()) bambuRemainMin = p["mc_remaining_time"].as<int>();
  if (!p["nozzle_temper"].isNull())     bambuNozzle    = p["nozzle_temper"].as<float>();
  if (!p["bed_temper"].isNull())        bambuBed       = p["bed_temper"].as<float>();
  if (!p["gcode_file"].isNull())        bambuFile      = p["gcode_file"].as<String>();
  bambuOnline = true;
}

void bambuConnect() {
  if (bambuIP.length() == 0 || bambuSerial.length() == 0) return;
  bambuTls.setInsecure();
  bambuMqtt.setServer(bambuIP.c_str(), 8883);
  bambuMqtt.setBufferSize(20480);   // Bambu report messages are large
  bambuMqtt.setCallback(bambuCallback);

  if (bambuMqtt.connect("deskbuddy", "bblp", bambuCode.c_str())) {
    bambuMqtt.subscribe(("device/" + bambuSerial + "/report").c_str());
    // ask for a full status push
    bambuMqtt.publish(("device/" + bambuSerial + "/request").c_str(),
                      "{\"pushing\":{\"sequence_id\":\"0\",\"command\":\"pushall\"}}");
  }
}

void bambuLoop() {
  if (!bambuEnabled) { if (bambuMqtt.connected()) bambuMqtt.disconnect(); bambuOnline = false; return; }
  if (WiFi.status() != WL_CONNECTED) return;

  if (!bambuMqtt.connected()) {
    bambuOnline = false;
    if (millis() - bambuLastAttempt >= BAMBU_RETRY_MS) {
      bambuLastAttempt = millis();
      bambuConnect();
    }
    return;
  }
  bambuMqtt.loop();
}

// ---- HP: poll the local status XML ----
static String xmlValue(const String& body, const char* tagEnd) {
  int i = body.indexOf(tagEnd);
  if (i < 0) return "";
  i += strlen(tagEnd);
  int j = body.indexOf('<', i);
  if (j < 0) return "";
  String v = body.substring(i, j);
  v.trim();
  return v;
}

void hpPoll() {
  if (!hpEnabled || hpIP.length() == 0) { hpOnline = false; return; }
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - hpLastPoll < HP_POLL_MS) return;
  hpLastPoll = millis();

  WiFiClient client;
  HTTPClient http;
  http.setConnectTimeout(2500);
  http.setTimeout(2500);

  if (http.begin(client, "http://" + hpIP + "/DevMgmt/ProductStatusDyn.xml")) {
    int code = http.GET();
    if (code == 200) {
      String body = http.getString();
      String st = xmlValue(body, "StatusCategory>");
      hpState = st.length() ? st : "ready";
      hpOnline = true;
    } else {
      hpOnline = false;
    }
    http.end();
  } else {
    hpOnline = false;
  }

  if (!hpOnline) return;

  // Best-effort ink/tank level (not all SmartTank models report it)
  if (http.begin(client, "http://" + hpIP + "/DevMgmt/ConsumableConfigDyn.xml")) {
    if (http.GET() == 200) {
      String body = http.getString();
      String lvl = xmlValue(body, "ConsumablePercentageLevelRemaining>");
      hpInk = lvl.length() ? lvl.toInt() : -1;
    }
    http.end();
  }
}

void devicesLoop() {
  bambuLoop();
  hpPoll();
}
