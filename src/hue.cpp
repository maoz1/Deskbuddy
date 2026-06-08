#include "hue.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// Defined in main.ino
extern Preferences prefs;

// ----- Shared state -----
bool hueEnabled = false;
String hueBridge = "";
String hueUser = "";
String hueStatusMsg = "";

// ----- Internal state -----
static bool hueSaved = false;       // do we have a captured pre-alert state?
static bool hueSavedAllOn = false;
static bool hueSavedOn = false;
static int hueSavedBri = 254;
static String hueSavedColor = "";   // JSON fragment to restore color (xy/ct/hue+sat)
static unsigned long hueLastFlashMs = 0;
static const unsigned long HUE_REFLASH_MS = 12000UL;

static bool hueRequest(const char* method, const String& path, const String& body, String& resp) {
  if (hueBridge.length() == 0) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClient client;
  HTTPClient http;
  http.setConnectTimeout(2500);
  http.setTimeout(2500);
  if (!http.begin(client, "http://" + hueBridge + path)) return false;

  int code;
  if (strcmp(method, "PUT") == 0)       code = http.PUT(body);
  else if (strcmp(method, "POST") == 0) code = http.POST(body);
  else                                  code = http.GET();

  if (code > 0) resp = http.getString();
  http.end();
  return code > 0;
}

// Auto-discover the bridge IP via Hue's cloud discovery service.
String hueFindBridge() {
  if (WiFi.status() != WL_CONNECTED) return "WiFi not connected";

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setConnectTimeout(6000);
  http.setTimeout(6000);
  if (!http.begin(client, "https://discovery.meethue.com/")) return "Could not start discovery";

  int code = http.GET();
  if (code != 200) { http.end(); return "Discovery service returned " + String(code); }
  String body = http.getString();
  http.end();

  JsonDocument doc;
  if (deserializeJson(doc, body)) return "Could not parse discovery response";
  if (!doc.is<JsonArray>() || doc.size() == 0) return "No bridge found on this network";

  String ip = doc[0]["internalipaddress"] | "";
  if (ip.length() == 0) return "Bridge found but no IP returned";

  hueBridge = ip;
  prefs.putString("hueBridge", hueBridge);
  return "Found bridge at " + ip + " - now press the bridge button and Pair";
}

// Press the bridge link button first, then call this.
String huePair() {
  if (hueBridge.length() == 0) return "Set the bridge IP first";
  String resp;
  if (!hueRequest("POST", "/api", "{\"devicetype\":\"deskbuddy#esp32\"}", resp))
    return "Bridge unreachable at " + hueBridge;

  JsonDocument doc;
  if (deserializeJson(doc, resp)) return "Unexpected bridge response";

  if (!doc[0]["success"]["username"].isNull()) {
    hueUser = doc[0]["success"]["username"].as<String>();
    prefs.putString("hueUser", hueUser);
    return "Paired successfully";
  }
  if ((doc[0]["error"]["type"] | 0) == 101)
    return "Press the round button on the Hue bridge, then tap Pair again";
  return "Pairing failed";
}

static void hueCaptureState() {
  hueSaved = false;
  if (!hueEnabled || hueUser.length() == 0) return;

  String resp;
  if (!hueRequest("GET", "/api/" + hueUser + "/groups/0", "", resp)) return;

  JsonDocument doc;
  if (deserializeJson(doc, resp)) return;

  hueSavedAllOn = doc["state"]["all_on"] | false;
  hueSavedOn    = doc["action"]["on"] | false;
  hueSavedBri   = doc["action"]["bri"] | 254;

  String mode = doc["action"]["colormode"] | "";
  if (mode == "xy" && doc["action"]["xy"].is<JsonArray>()) {
    JsonArray xy = doc["action"]["xy"];
    hueSavedColor = "\"xy\":[" + String((float)xy[0], 4) + "," + String((float)xy[1], 4) + "]";
  } else if (mode == "ct") {
    hueSavedColor = "\"ct\":" + String((int)(doc["action"]["ct"] | 366));
  } else if (!doc["action"]["hue"].isNull()) {
    hueSavedColor = "\"hue\":" + String((int)(doc["action"]["hue"] | 0)) +
                    ",\"sat\":" + String((int)(doc["action"]["sat"] | 0));
  } else {
    hueSavedColor = "";
  }
  hueSaved = true;
}

void hueAlertOn() {
  if (!hueEnabled || hueUser.length() == 0) return;
  hueCaptureState();
  String resp;
  hueRequest("PUT", "/api/" + hueUser + "/groups/0/action",
             "{\"on\":true,\"bri\":254,\"hue\":0,\"sat\":254,\"alert\":\"lselect\"}", resp);
  hueLastFlashMs = millis();
}

void hueReflash() {
  if (!hueEnabled || hueUser.length() == 0) return;
  if (millis() - hueLastFlashMs < HUE_REFLASH_MS) return;
  hueLastFlashMs = millis();
  String resp;
  hueRequest("PUT", "/api/" + hueUser + "/groups/0/action",
             "{\"on\":true,\"bri\":254,\"hue\":0,\"sat\":254,\"alert\":\"lselect\"}", resp);
}

void hueAlertOff() {
  if (!hueEnabled || hueUser.length() == 0) return;
  String body;
  if (hueSaved && !hueSavedAllOn && !hueSavedOn) {
    body = "{\"on\":false,\"alert\":\"none\"}";
  } else {
    body = "{\"on\":true,\"bri\":" + String(hueSavedBri) +
           (hueSavedColor.length() ? ("," + hueSavedColor) : "") +
           ",\"alert\":\"none\"}";
  }
  String resp;
  hueRequest("PUT", "/api/" + hueUser + "/groups/0/action", body, resp);
  hueSaved = false;
}

String hueTest() {
  if (hueBridge.length() == 0) return "Set the bridge IP and tap Save first";
  if (hueUser.length() == 0)   return "Not paired yet - press the bridge button and tap Pair";

  String resp;
  if (!hueRequest("GET", "/api/" + hueUser + "/groups/0", "", resp))
    return "Bridge unreachable at " + hueBridge;
  if (resp.indexOf("unauthorized") >= 0)
    return "Bridge rejected the saved token - pair again";

  bool wasEnabled = hueEnabled;
  hueEnabled = true;          // allow the test even before the checkbox is saved
  hueAlertOn();
  delay(3500);
  hueAlertOff();
  hueEnabled = wasEnabled;
  return "Test OK - lights flashed red and restored";
}
