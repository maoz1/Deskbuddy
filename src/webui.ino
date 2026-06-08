// =========================================================
// WEB SERVER / UI (moved from main.ino - shares the single Arduino TU)
// =========================================================

void handleRoot() {
  String accent = prefs.getString("accent", "cyan");
  String bg     = prefs.getString("bg", "slate");
  String txt    = prefs.getString("text", "standard");
  String units  = prefs.getString("units", "metric");
  String region = prefs.getString("region", "europe");
  String tz     = sanitizeTimezoneKey(prefs.getString("tz", "europe_central"));
  String nickname = prefs.getString("nickname", "");
  bool flashMode = prefs.getBool("flashMode", false);
  bool prchEn = prefs.getBool("prchEn", false);
  String prchArea = prefs.getString("prchArea", "");
  String prchLabel = prefs.getString("prchLabel", "");
  bool hueEn = prefs.getBool("hueEn", false);
  String hueBridgeVal = prefs.getString("hueBridge", "");
  bool huePaired = prefs.getString("hueUser", "").length() > 0;
  String homeSlotKeys[HOME_SLOT_COUNT];
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    homeSlotKeys[i] = prefs.getString((String("homeSlot") + String(i)).c_str(), homeWidgetKey(homeWidgetSlots[i]));
  }

  const char* swKeys[] = {"standard","ice","white","cyan","mint","green","blue","purple","pink","orange","amber","red"};
  const int swCount = 12;
  const char* bgKeys[] = {"slate","deep","nordic","forest","coffee","soft","midnight","graphite","garnet","ochre"};
  const int bgCount = 10;

  String page;
  page.reserve(23000);

  page += "<!doctype html><html><head>";
  page += "<meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>Deskbuddy</title>";
  page += "<style>";
  page += ":root{--bg:#0b1018;--card:#161b24;--card2:#0e141d;--line:#283545;--text:#e7eef7;--dim:#93a4b8;--accent:#38bdf8;--accent2:#0ea5e9;color-scheme:dark;}";
  page += "*{box-sizing:border-box;}";
  page += "body{margin:0;background:var(--bg);color:var(--text);font-family:system-ui,-apple-system,'Segoe UI',Roboto,sans-serif;font-size:15px;}";
  page += ".topbar{position:sticky;top:0;z-index:20;display:flex;align-items:center;gap:12px;padding:12px 16px;background:rgba(11,16,24,.9);backdrop-filter:blur(10px);border-bottom:1px solid var(--line);}";
  page += ".brand{font-weight:800;font-size:18px;letter-spacing:.02em;}";
  page += ".ip{font-size:12px;color:var(--dim);padding:4px 10px;border:1px solid var(--line);border-radius:999px;white-space:nowrap;}";
  page += ".spacer{flex:1;}";
  page += ".save-btn{background:var(--accent);color:#001018;border:none;font-weight:800;padding:10px 20px;border-radius:10px;cursor:pointer;font:inherit;}";
  page += ".save-btn:hover{background:var(--accent2);}";
  page += ".tabs{position:sticky;top:57px;z-index:19;display:flex;gap:6px;overflow-x:auto;padding:10px 12px;background:var(--bg);border-bottom:1px solid var(--line);}";
  page += ".tab{flex:0 0 auto;background:transparent;border:1px solid transparent;color:var(--dim);padding:9px 16px;border-radius:999px;cursor:pointer;font:inherit;font-weight:600;white-space:nowrap;}";
  page += ".tab:hover{color:var(--text);}";
  page += ".tab.active{background:var(--card);color:var(--text);border-color:var(--line);}";
  page += ".wrap{max-width:760px;margin:0 auto;padding:18px 16px 70px;}";
  page += ".pane{display:none;}.pane.active{display:block;}";
  page += ".card{background:var(--card);border:1px solid var(--line);border-radius:16px;padding:18px;margin-bottom:16px;}";
  page += ".card h2{margin:0 0 4px;font-size:17px;}";
  page += ".card .desc{margin:0 0 16px;color:var(--dim);font-size:13px;line-height:1.5;}";
  page += "label.lbl{display:block;font-size:13px;font-weight:600;color:var(--dim);margin:0 0 7px;}";
  page += "input,select,textarea{width:100%;border-radius:10px;border:1px solid var(--line);background:var(--card2);color:var(--text);padding:11px 12px;font:inherit;}";
  page += "textarea{min-height:150px;resize:vertical;}";
  page += ".row{display:grid;grid-template-columns:1fr 1fr;gap:14px;}";
  page += ".row3{display:grid;grid-template-columns:1fr 1fr 1fr;gap:14px;}";
  page += ".field{margin-bottom:14px;}";
  page += ".note{font-size:12px;color:#7f93a8;margin-top:8px;line-height:1.5;}";
  page += ".check{display:flex;align-items:center;gap:10px;margin:6px 0 14px;color:var(--text);}";
  page += ".check input{width:auto;}";
  page += ".btn{background:var(--accent);color:#001018;border:none;font-weight:700;padding:11px 16px;border-radius:10px;cursor:pointer;font:inherit;}";
  page += ".btn.sec{background:#1f2937;color:var(--text);border:1px solid var(--line);}";
  page += ".btn.red{background:#dc2626;color:#fff;}";
  page += ".btn.green{background:#0f8a6a;color:#fff;}";
  page += ".btn.blue{background:#2d6cdf;color:#fff;}";
  page += ".btn.purple{background:#7c3aed;color:#fff;}";
  page += ".btnrow{display:flex;flex-wrap:wrap;gap:10px;margin-top:8px;}";
  page += ".swatch-row{display:flex;flex-wrap:wrap;gap:8px;}";
  page += ".swatch{width:26px;height:26px;border-radius:999px;border:1px solid rgba(255,255,255,.18);cursor:pointer;position:relative;}";
  page += ".swatch input{display:none;}";
  page += ".swatch.active{box-shadow:0 0 0 2px var(--accent),0 0 0 5px rgba(56,189,248,.2);}";
  page += ".colorgroup{margin-bottom:18px;}";
  page += ".colorhead{display:flex;justify-content:space-between;margin-bottom:8px;}";
  page += ".colorhead .val{font-size:12px;color:var(--dim);}";
  page += ".timergrid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px;}";
  page += ".tslot{border:1px solid var(--line);border-radius:10px;background:var(--card2);padding:10px;}";
  page += ".tslot .h{font-size:12px;color:var(--dim);margin-bottom:6px;}";
  page += ".tslot input{text-align:center;font-weight:700;}";
  page += ".status{padding:10px 12px;border-radius:10px;margin-bottom:14px;font-weight:600;font-size:13px;}";
  page += ".status.ok{background:#10331f;border:1px solid #34d399;color:#6ee7b7;}";
  page += ".status.bad{background:#3a1414;border:1px solid #f87171;color:#fca5a5;}";
  page += ".pill{display:inline-block;padding:5px 12px;border-radius:999px;font-size:13px;font-weight:600;}";
  page += ".pill.ok{background:#10331f;color:#6ee7b7;border:1px solid #34d399;}";
  page += ".pill.bad{background:#3a1414;color:#fca5a5;border:1px solid #f87171;}";
  page += "@media(max-width:620px){.row,.row3,.timergrid{grid-template-columns:1fr;}}";
  page += "</style></head><body>";

  // ---- Sticky top bar with always-visible Save ----
  page += "<div class='topbar'><span class='brand'>Deskbuddy</span><span class='ip'>" + WiFi.localIP().toString() + "</span><span class='spacer'></span>";
  page += "<button class='save-btn' type='submit' form='mainform'>Save</button></div>";

  // ---- Tab bar ----
  page += "<div class='tabs'>";
  page += "<button type='button' class='tab active' data-tab='notes'>Notes</button>";
  page += "<button type='button' class='tab' data-tab='appearance'>Appearance</button>";
  page += "<button type='button' class='tab' data-tab='general'>General</button>";
  page += "<button type='button' class='tab' data-tab='alerts'>Alerts</button>";
  page += "<button type='button' class='tab' data-tab='hue'>Hue</button>";
  page += "</div>";

  page += "<form id='mainform' method='POST' action='/save'><div class='wrap'>";

  // ===== NOTES =====
  page += "<div class='pane active' data-pane='notes'><div class='card'>";
  page += "<h2>Notes</h2><div class='desc'>Short notes synced to the device screen.</div>";
  page += "<textarea name='notes' maxlength='700'>" + htmlEscape(notesText) + "</textarea>";
  page += "<div class='note'>Saved notes show up on the device right away.</div>";
  page += "</div></div>";

  // ===== APPEARANCE =====
  page += "<div class='pane' data-pane='appearance'>";
  page += "<div class='card'><h2>Colors</h2><div class='desc'>Accent, text, and background theme for the display.</div>";
  // accent
  page += "<div class='colorgroup'><div class='colorhead'><label class='lbl'>Accent</label><span class='val' id='accent-value'>" + accent + "</span></div><div class='swatch-row'>";
  for (int i = 0; i < swCount; i++)
    page += "<label class='swatch" + String(accent==swKeys[i]?" active":"") + "' style='background:" + accentPreviewCss(swKeys[i]) + ";'><input type='radio' name='accent' value='" + swKeys[i] + "'" + String(accent==swKeys[i]?" checked":"") + "></label>";
  page += "</div></div>";
  // text
  page += "<div class='colorgroup'><div class='colorhead'><label class='lbl'>Text</label><span class='val' id='text-value'>" + txt + "</span></div><div class='swatch-row'>";
  for (int i = 0; i < swCount; i++)
    page += "<label class='swatch" + String(txt==swKeys[i]?" active":"") + "' style='background:" + accentPreviewCss(swKeys[i]) + ";'><input type='radio' name='text' value='" + swKeys[i] + "'" + String(txt==swKeys[i]?" checked":"") + "></label>";
  page += "</div></div>";
  // bg
  page += "<div class='colorgroup'><div class='colorhead'><label class='lbl'>Theme</label><span class='val' id='bg-value'>" + bg + "</span></div><div class='swatch-row'>";
  for (int i = 0; i < bgCount; i++)
    page += "<label class='swatch" + String(bg==bgKeys[i]?" active":"") + "' style='background:" + themePreviewCss(bgKeys[i]) + ";'><input type='radio' name='bg' value='" + bgKeys[i] + "'" + String(bg==bgKeys[i]?" checked":"") + "></label>";
  page += "</div></div>";
  page += "</div>";
  // widgets
  page += "<div class='card'><h2>Home widgets</h2><div class='desc'>Which widgets appear in the four Home slots below the clock.</div><div class='row'>";
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    page += "<div class='field'><label class='lbl'>" + String(homeSlotLabel(i)) + "</label><select name='homeSlot" + String(i) + "'>";
    appendHomeWidgetOptions(page, homeSlotKeys[i]);
    page += "</select></div>";
  }
  page += "</div></div></div>";

  // ===== GENERAL =====
  page += "<div class='pane' data-pane='general'>";
  page += "<div class='card'><h2>Device</h2><div class='desc'>Nickname and power saving.</div><div class='row'>";
  page += "<div class='field'><label class='lbl'>Buddy nickname</label><input name='nickname' maxlength='24' value='" + htmlEscape(nickname) + "'></div>";
  page += "<div class='field'><label class='lbl'>Auto sleep</label><select name='sleepMin'>";
  page += "<option value='0'"  + String(sleepIntervalMin==0?" selected":"")  + ">Never</option>";
  page += "<option value='1'"  + String(sleepIntervalMin==1?" selected":"")  + ">1 minute</option>";
  page += "<option value='5'"  + String(sleepIntervalMin==5?" selected":"")  + ">5 minutes</option>";
  page += "<option value='10'" + String(sleepIntervalMin==10?" selected":"") + ">10 minutes</option>";
  page += "<option value='30'" + String(sleepIntervalMin==30?" selected":"") + ">30 minutes</option>";
  page += "<option value='60'" + String(sleepIntervalMin==60?" selected":"") + ">1 hour</option>";
  page += "</select></div>";
  page += "</div><div class='note'>Sleep dims the screen first, then turns it fully off after 60 seconds.</div></div>";

  page += "<div class='card'><h2>Units &amp; format</h2><div class='desc'>Measurement, date format, and time zone.</div><div class='row3'>";
  page += "<div class='field'><label class='lbl'>Measurement</label><select name='units'>";
  page += "<option value='metric'"   + String(units=="metric"?" selected":"")   + ">Celsius / mm</option>";
  page += "<option value='imperial'" + String(units=="imperial"?" selected":"") + ">Fahrenheit / in</option>";
  page += "</select></div>";
  page += "<div class='field'><label class='lbl'>Date format</label><select name='region'>";
  page += "<option value='europe'" + String(region=="europe"?" selected":"") + ">dd.mm.yyyy</option>";
  page += "<option value='us'" + String(region=="us"?" selected":"") + ">mm/dd/yyyy</option>";
  page += "</select></div>";
  page += "<div class='field'><label class='lbl'>Time zone</label><select name='tz'>";
  appendTimezoneOptions(page, tz);
  page += "</select></div></div></div>";

  page += "<div class='card'><h2>Location</h2><div class='desc'>Used for weather data and sun times.</div><div class='row3'>";
  page += "<div class='field'><label class='lbl'>Name</label><input name='locname' value='" + htmlEscape(locationName) + "'></div>";
  page += "<div class='field'><label class='lbl'>Latitude</label><input name='lat' value='" + String(LAT, 6) + "'></div>";
  page += "<div class='field'><label class='lbl'>Longitude</label><input name='lng' value='" + String(LNG, 6) + "'></div>";
  page += "</div><div class='note'>Example Berlin: latitude 52.5200, longitude 13.4050.</div></div>";

  page += "<div class='card'><h2>Timer</h2><div class='desc'>The six quick timers shown in the device popup menu.</div><div class='timergrid'>";
  for (int i = 0; i < 6; i++)
    page += "<div class='tslot'><div class='h'>Slot " + String(i + 1) + "</div><input type='number' min='1' max='180' name='timer" + String(i) + "' value='" + String(timerPresetMin[i]) + "'></div>";
  page += "</div><label class='check' style='margin-top:14px;'><input type='checkbox' name='flashMode' value='1'" + String(flashMode ? " checked" : "") + ">Flash screen when timer ends</label></div></div>";

  // ===== ALERTS =====
  page += "<div class='pane' data-pane='alerts'><div class='card'>";
  page += "<h2>Red Alert (Home Front Command)</h2>";
  page += "<div class='desc'>Polls the unofficial Pikud HaOref feed and shows a full-screen alarm when your area is targeted. <b>Unofficial &mdash; not a substitute for the official Home Front Command app or sirens.</b></div>";
  page += "<label class='check'><input type='checkbox' name='prchEn' value='1'" + String(prchEn ? " checked" : "") + ">Enable red alert monitoring</label>";
  page += "<div class='row'>";
  page += "<div class='field'><label class='lbl'>Area match (Hebrew)</label><input name='prchArea' value='" + htmlEscape(prchArea) + "' placeholder='example: תל אביב'></div>";
  page += "<div class='field'><label class='lbl'>Display label (English)</label><input name='prchLabel' maxlength='40' value='" + htmlEscape(prchLabel) + "' placeholder='example: Tel Aviv'></div>";
  page += "</div><div class='note'>Leave the area blank to alert on <b>any</b> alert in Israel. Matching is a substring of the official Hebrew area name.</div>";
  page += "<div class='btnrow'><button class='btn red' type='submit' formaction='/testalert'>Test red alert on device</button></div>";
  page += "<div class='note'>Triggers the full-screen alarm and buzzer now, to verify the screen and sound. Save your settings first (top bar).</div>";
  page += "</div></div>";

  // ===== HUE =====
  page += "<div class='pane' data-pane='hue'><div class='card'>";
  page += "<h2>Philips Hue lights</h2>";
  page += "<div class='desc'>Flash all Hue lights red during a red alert, then restore them. The bridge must be on the same network as Deskbuddy.</div>";
  if (hueStatusMsg.length()) {
    bool ok = (hueStatusMsg.indexOf("OK") >= 0) || (hueStatusMsg.indexOf("success") >= 0) || (hueStatusMsg.indexOf("Found") >= 0);
    page += "<div class='status " + String(ok ? "ok" : "bad") + "'>" + htmlEscape(hueStatusMsg) + "</div>";
  }
  page += "<label class='check'><input type='checkbox' name='hueEn' value='1'" + String(hueEn ? " checked" : "") + ">Flash Hue lights on alert</label>";
  page += "<div class='field'><label class='lbl'>Bridge IP</label><input name='hueBridge' value='" + htmlEscape(hueBridgeVal) + "' placeholder='192.168.1.10'></div>";
  page += "<div class='field'><label class='lbl'>Pairing status</label> <span class='pill " + String(huePaired ? "ok" : "bad") + "'>" + String(huePaired ? "Paired" : "Not paired") + "</span></div>";
  page += "<div class='btnrow'>";
  page += "<button class='btn green' type='submit' formaction='/huefind'>1. Find bridge</button>";
  page += "<button class='btn blue' type='submit' formaction='/huepair'>2. Pair</button>";
  page += "<button class='btn purple' type='submit' formaction='/huetest'>3. Test lights</button>";
  page += "</div>";
  page += "<div class='note'><b>Find</b> auto-fills the bridge IP. Then press the round button on the Hue bridge and tap <b>Pair</b> within 30 seconds. <b>Test</b> flashes the lights red to confirm.</div>";
  page += "</div></div>";

  page += "</div></form>";

  // ---- Scripts ----
  page += "<script>";
  page += "var colorNames={accent:{standard:'Standard',ice:'Ice',white:'White',cyan:'Cyan',mint:'Mint',green:'Green',blue:'Blue',purple:'Purple',pink:'Pink',orange:'Orange',amber:'Amber',red:'Red'},text:{standard:'Standard',ice:'Ice',white:'White',cyan:'Cyan',mint:'Mint',green:'Green',blue:'Blue',purple:'Purple',pink:'Pink',orange:'Orange',amber:'Amber',red:'Red'},bg:{slate:'Slate',deep:'Deep black',nordic:'Nordic blue',forest:'Forest',coffee:'Coffee',soft:'Soft dark',midnight:'Midnight',graphite:'Graphite',garnet:'Garnet',ochre:'Ochre'}};";
  page += "var tabKey='deskbuddy-tab-v2';";
  page += "function showTab(id){document.querySelectorAll('.tab').forEach(function(t){t.classList.toggle('active',t.dataset.tab===id);});document.querySelectorAll('.pane').forEach(function(p){p.classList.toggle('active',p.dataset.pane===id);});try{localStorage.setItem(tabKey,id);}catch(e){}}";
  page += "document.querySelectorAll('.tab').forEach(function(t){t.addEventListener('click',function(){showTab(t.dataset.tab);});});";
  String initialTab = hueStatusMsg.length() ? "hue" : "";
  page += "var initialTab='" + initialTab + "';var savedTab='';try{savedTab=localStorage.getItem(tabKey)||'';}catch(e){}var useTab=initialTab||savedTab;";
  page += "if(useTab&&document.querySelector('.pane[data-pane=\"'+useTab+'\"]')){showTab(useTab);}";
  page += "document.querySelectorAll('.swatch input').forEach(function(input){input.addEventListener('change',function(){document.querySelectorAll('.swatch input[name=\"'+input.name+'\"]').forEach(function(peer){peer.closest('.swatch').classList.toggle('active',peer.checked);});var v=document.getElementById(input.name+'-value');if(v&&colorNames[input.name]&&colorNames[input.name][input.value]){v.textContent=colorNames[input.name][input.value];}});});";
  page += "</script>";

  hueStatusMsg = "";  // shown once
  page += "</body></html>";

  server.send(200, "text/html; charset=utf-8", page);
}

void handleSave() {
  String newNotes  = server.hasArg("notes") ? server.arg("notes") : notesText;
  String newAccent = server.hasArg("accent") ? server.arg("accent") : "cyan";
  String newBg     = server.hasArg("bg") ? server.arg("bg") : "slate";
  String newText   = server.hasArg("text") ? server.arg("text") : "standard";
  String newUnits  = server.hasArg("units") ? server.arg("units") : "metric";
  String newRegion = server.hasArg("region") ? server.arg("region") : "europe";
  String newTz     = server.hasArg("tz") ? server.arg("tz") : timezoneKey;
  String newLoc    = server.hasArg("locname") ? server.arg("locname") : locationName;
  String newNickname = server.hasArg("nickname") ? server.arg("nickname") : buddyNickname;
  HomeWidgetType newHomeSlots[HOME_SLOT_COUNT];
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    String key = String("homeSlot") + String(i);
    String currentKey = homeWidgetKey(homeWidgetSlots[i]);
    newHomeSlots[i] = homeWidgetFromKey(server.hasArg(key) ? server.arg(key) : currentKey);
  }

  float newLat = server.hasArg("lat") ? server.arg("lat").toFloat() : LAT;
  float newLng = server.hasArg("lng") ? server.arg("lng").toFloat() : LNG;

  newNotes.trim();
  newLoc.trim();
  newNickname.trim();

  if (newNotes.length() == 0) newNotes = "No notes yet.";
  if (newNotes.length() > 700) newNotes = newNotes.substring(0, 700);
  if (newLoc.length() == 0) newLoc = "Unknown";
  if (newNickname.length() > 24) newNickname = newNickname.substring(0, 24);
  if (newUnits != "metric" && newUnits != "imperial") newUnits = "metric";
  if (newRegion != "europe" && newRegion != "us") newRegion = "europe";
  newTz = sanitizeTimezoneKey(newTz);

  int newSleepMin = server.hasArg("sleepMin") ? server.arg("sleepMin").toInt() : sleepIntervalMin;
  sleepIntervalMin = constrain(newSleepMin, 0, 120);
  bool newFlashMode = server.hasArg("flashMode");

  prchEnabled   = server.hasArg("prchEn");
  prchAreaMatch = server.hasArg("prchArea") ? server.arg("prchArea") : prchAreaMatch;
  prchAreaLabel = server.hasArg("prchLabel") ? server.arg("prchLabel") : prchAreaLabel;
  prchAreaMatch.trim();
  prchAreaLabel.trim();
  if (prchAreaLabel.length() > 40) prchAreaLabel = prchAreaLabel.substring(0, 40);

  hueEnabled = server.hasArg("hueEn");
  hueBridge  = server.hasArg("hueBridge") ? server.arg("hueBridge") : hueBridge;
  hueBridge.trim();

  bool locationChanged =
    (fabsf(newLat - LAT) > 0.0001f) ||
    (fabsf(newLng - LNG) > 0.0001f) ||
    (newLoc != locationName);

  notesText = newNotes;
  buddyNickname = newNickname;
  locationName = newLoc;
  LAT = newLat;
  LNG = newLng;
  unitKey = newUnits;
  regionFormatKey = newRegion;
  timezoneKey = newTz;
  flashModeEnabled = newFlashMode;
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    homeWidgetSlots[i] = newHomeSlots[i];
  }

  for (int i = 0; i < 6; i++) {
    String key = String("timer") + String(i);
    int currentValue = timerPresetMin[i];
    int nextValue = server.hasArg(key) ? server.arg(key).toInt() : currentValue;
    timerPresetMin[i] = sanitizeTimerMinutes(nextValue);
  }

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
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    String key = String("homeSlot") + String(i);
    prefs.putString(key.c_str(), homeWidgetKey(homeWidgetSlots[i]));
  }
  for (int i = 0; i < 6; i++) {
    String key = String("timer") + String(i);
    prefs.putInt(key.c_str(), timerPresetMin[i]);
  }

  applyThemeByKey(newAccent, newBg);
  applyTextColorByKey(newText);
  applyDeviceTimezoneByKey(timezoneKey);
  if (!sleepDimmed && !sleepOff) setBacklight(BL_FULL);

  notesDirty = true;
  pageDirty = true;
  dataDirty = true;

  cacheClock = "";
  cacheHomeEmpty1 = "";
  cacheHomeEmpty2 = "";
  cacheFocusTimer = "";
  cacheTimerMenu = "";
  cacheTimerDone = "";
  for (int i = 0; i < HOME_SLOT_COUNT; i++) {
    cacheHomeSlots[i] = "";
  }

  lastTempText = "";
  lastRainText = "";
  lastKpText = "";
  lastKpLevelText = "";
  lastWindText = "";
  lastWindDirText = "";
  lastNextSunLabel = "";
  lastNextSunTime = "";
  lastUptimeText = "";

  if (locationChanged) resetDataCaches();

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleTestAlert() {
  prchLastAlertId = "test-" + String(millis());
  triggerRedAlert();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleHuePair() {
  hueStatusMsg = huePair();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleHueTest() {
  hueStatusMsg = hueTest();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleHueFind() {
  hueStatusMsg = hueFindBridge();
  server.sendHeader("Location", "/");
  server.send(303);
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/testalert", HTTP_POST, handleTestAlert);
  server.on("/huepair", HTTP_POST, handleHuePair);
  server.on("/huetest", HTTP_POST, handleHueTest);
  server.on("/huefind", HTTP_POST, handleHueFind);
  server.begin();
}
