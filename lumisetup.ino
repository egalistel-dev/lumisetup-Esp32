/*
 * ╔══════════════════════════════════════════════════╗
 * ║         Lampe Connectée — LumiSetup              ║
 * ║   ESP32 WROOM-32 + HC-SR501 + WS2812B            ║
 * ║         Fichier unique — Version ESP32            ║
 * ╠══════════════════════════════════════════════════╣
 * ║  Auteur  : Egalistel                             ║
 * ║  Site    : https://egamaker.be                   ║
 * ╚══════════════════════════════════════════════════╝
 *
 * Bibliothèques requises :
 *   - FastLED           (par Daniel Garcia)
 *   - ESPAsyncWebServer (par me-no-dev, via GitHub)
 *   - AsyncTCP          (par me-no-dev, via GitHub)
 *   - ArduinoJson       v6 (par Benoit Blanchon)
 *   - AsyncMqttClient   (par Marvin Roger, via GitHub)
 *
 * Package ESP32 Arduino IDE :
 *   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *   Carte : ESP32 Dev Module
 *
 * !! NE PAS installer WiFiManager — conflit avec ESPAsyncWebServer !!
 *
 * Câblage ESP32 WROOM-32 :
 *   HC-SR501 VCC → 3.3V  |  GND → GND  |  OUT → GPIO14
 *   WS2812B  DIN → GPIO12 |  5V  → Alim 5V externe
 *   GND alim 5V → GND ESP32 (GND COMMUN obligatoire)
 *   Alimentation ESP32 : 5V → VIN (ou via USB)
 *
 * Premier démarrage :
 *   → Rejoindre le WiFi "LumiSetup" (mdp: lumi1234)
 *   → Ouvrir http://192.168.4.1 pour configurer le WiFi
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <time.h>
#include <AsyncMqttClient.h>
#include <HTTPClient.h>

// ─────────────────────────────────────────────
//  PINS
// ─────────────────────────────────────────────
#define PIR_PIN      14
#define LED_DATA_PIN 12

// ─────────────────────────────────────────────
//  CONSTANTES
// ─────────────────────────────────────────────
#define MAX_LEDS    300
#define CONFIG_FILE "/config.json"
#define WIFI_FILE   "/wifi.json"
#define AP_NAME     "LumiSetup"
#define AP_PASS     "lumi1234"
#define AP_IP_STR   "192.168.4.1"
#define HOSTNAME    "lumisetup"

// ── Topics MQTT ──────────────────────────────
#define MQTT_STATE_LEDS       "lumisetup/state/leds"
#define MQTT_STATE_PIR        "lumisetup/state/pir"
#define MQTT_STATE_MODE       "lumisetup/state/mode"
#define MQTT_STATE_OVERRIDE   "lumisetup/state/override"
#define MQTT_STATE_SYSTEM     "lumisetup/state/system"
#define MQTT_STATE_BRIGHTNESS "lumisetup/state/brightness"
#define MQTT_STATE_COLOR      "lumisetup/state/color"
#define MQTT_CMD_SYSTEM       "lumisetup/cmd/system"
#define MQTT_CMD_MODE         "lumisetup/cmd/mode"
#define MQTT_CMD_OVERRIDE     "lumisetup/cmd/override"
#define MQTT_CMD_BRIGHTNESS   "lumisetup/cmd/brightness"
#define MQTT_CMD_COLOR        "lumisetup/cmd/color"
#define MQTT_CMD_SCHEDULE     "lumisetup/cmd/schedule"

// ── Timezones disponibles ────────────────────
const char* TIMEZONES[] = {
  "Europe/Brussels|CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Paris|CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/London|GMT0BST,M3.5.0/1,M10.5.0",
  "Europe/Amsterdam|CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Berlin|CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Madrid|CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Rome|CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Zurich|CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Lisbon|WET0WEST,M3.5.0/1,M10.5.0",
  "Europe/Helsinki|EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Athens|EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Bucharest|EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Moscow|MSK-3",
  "America/New_York|EST5EDT,M3.2.0,M11.1.0",
  "America/Chicago|CST6CDT,M3.2.0,M11.1.0",
  "America/Denver|MST7MDT,M3.2.0,M11.1.0",
  "America/Los_Angeles|PST8PDT,M3.2.0,M11.1.0",
  "America/Toronto|EST5EDT,M3.2.0,M11.1.0",
  "America/Vancouver|PST8PDT,M3.2.0,M11.1.0",
  "America/Sao_Paulo|<-03>3",
  "America/Argentina/Buenos_Aires|<-03>3",
  "Asia/Dubai|<+04>-4",
  "Asia/Kolkata|IST-5:30",
  "Asia/Bangkok|<+07>-7",
  "Asia/Singapore|<+08>-8",
  "Asia/Shanghai|CST-8",
  "Asia/Tokyo|JST-9",
  "Asia/Seoul|KST-9",
  "Australia/Sydney|AEST-10AEDT,M10.1.0,M4.1.0/3",
  "Pacific/Auckland|NZST-12NZDT,M9.5.0,M4.1.0/3",
  "Africa/Johannesburg|SAST-2",
  "Africa/Cairo|EET-2",
  "UTC|UTC0"
};
const int TZ_COUNT = sizeof(TIMEZONES) / sizeof(TIMEZONES[0]);

// ─────────────────────────────────────────────
//  CONFIG
// ─────────────────────────────────────────────
struct Config {
  uint16_t numLeds      = 30;
  uint8_t  red          = 255;
  uint8_t  green        = 200;
  uint8_t  blue         = 100;
  uint8_t  brightness   = 80;
  uint16_t duration     = 120;
  bool     fadeEffect   = true;
  uint8_t  startHour    = 18;
  uint8_t  startMin     = 0;
  uint8_t  endHour      = 23;
  uint8_t  endMin       = 0;
  uint8_t  pirDebounce  = 3;
  bool     systemActive = true;
  bool     modeOn       = false;
  bool     langFR       = true;
  String   timezone     = "Europe/Brussels";
  uint8_t  scheduleMode = 0;  // 0=Manuel, 1=Soleil via HA, 2=Soleil standalone
  String   city         = "Brussels";
  bool     mqttEnabled  = false;
  String   mqttHost     = "";
  uint16_t mqttPort     = 1883;
  String   mqttUser     = "";
  String   mqttPass     = "";
  String   mqttId       = "lumisetup";
};

// ─────────────────────────────────────────────
//  VARIABLES
// ─────────────────────────────────────────────
Config         cfg;
CRGB           leds[MAX_LEDS];
AsyncWebServer server(80);
DNSServer      dnsServer;
AsyncMqttClient mqttClient;
TimerHandle_t  mqttReconnectTimer = nullptr;

bool          portalMode         = false;
bool          ledsOn             = false;
bool          overrideOn         = false;
bool          pirState           = false;
unsigned long lightOffTime       = 0;
unsigned long lastPirTime        = 0;
bool          pendingLeds        = false;
bool          pendingLedsOn      = false;
bool          pendingLedsSmooth  = true;
bool          lastLedsOn         = false;
bool          lastPirState       = false;
unsigned long lastSunUpdate      = 0;
#define SUN_UPDATE_INTERVAL 86400000UL

// ─────────────────────────────────────────────
//  PROTOTYPES
// ─────────────────────────────────────────────
void loadConfig();
void saveConfig();
void loadWifi(String &ssid, String &pass);
void saveWifi(const String &ssid, const String &pass);
void startPortalMode();
void startNormalMode();
void applyLeds(bool on, bool smooth = true);
bool isInTimeRange();
void handlePir();
void setupPortalRoutes();
void setupAppRoutes();
String buildStatusJson();
String buildConfigJson();
void mqttPublishAll();
void mqttConnect();
void applyTimezone();
bool fetchSunriseSunset();
struct tm getLocalTime_();

// ─────────────────────────────────────────────
//  TIMEZONE
// ─────────────────────────────────────────────
String getTZPosix(const String& name) {
  for (int i = 0; i < TZ_COUNT; i++) {
    String entry = String(TIMEZONES[i]);
    int sep = entry.indexOf('|');
    if (entry.substring(0, sep) == name) return entry.substring(sep + 1);
  }
  return "CET-1CEST,M3.5.0,M10.5.0/3";
}

void applyTimezone() {
  String posix = getTZPosix(cfg.timezone);
  configTzTime(posix.c_str(), "pool.ntp.org", "time.google.com");
  Serial.println("[TZ] " + cfg.timezone + " → " + posix);
}

struct tm getLocalTime_() {
  struct tm ti;
  if (!getLocalTime(&ti)) memset(&ti, 0, sizeof(ti));
  return ti;
}

// ─────────────────────────────────────────────
//  SUNRISE/SUNSET STANDALONE
// ─────────────────────────────────────────────
bool fetchSunriseSunset() {
  if (cfg.city.length() == 0) return false;
  Serial.println("[SUN] Récupération pour : " + cfg.city);
  HTTPClient http;
  String url = "https://wttr.in/" + cfg.city + "?format=j1";
  http.begin(url);
  http.setTimeout(8000);
  int code = http.GET();
  if (code != 200) { Serial.println("[SUN] Erreur HTTP : " + String(code)); http.end(); return false; }
  String payload = http.getString();
  http.end();
  StaticJsonDocument<4096> doc;
  if (deserializeJson(doc, payload) != DeserializationError::Ok) { Serial.println("[SUN] Erreur JSON"); return false; }
  const char* sunrise = doc["weather"][0]["astronomy"][0]["sunrise"];
  const char* sunset  = doc["weather"][0]["astronomy"][0]["sunset"];
  if (!sunrise || !sunset) { Serial.println("[SUN] Données manquantes"); return false; }
  auto parseWttr = [](const char* t, uint8_t &h, uint8_t &m) {
    int hour = 0, min = 0; char ampm[3] = "";
    sscanf(t, "%d:%d %s", &hour, &min, ampm);
    if (strcmp(ampm, "PM") == 0 && hour != 12) hour += 12;
    if (strcmp(ampm, "AM") == 0 && hour == 12) hour = 0;
    h = hour; m = min;
  };
  parseWttr(sunset,  cfg.startHour, cfg.startMin);
  parseWttr(sunrise, cfg.endHour,   cfg.endMin);
  Serial.printf("[SUN] Coucher: %02d:%02d → Lever: %02d:%02d\n", cfg.startHour, cfg.startMin, cfg.endHour, cfg.endMin);
  return true;
}

// ─────────────────────────────────────────────
//  MQTT CALLBACKS
// ─────────────────────────────────────────────
void onMqttConnect(bool sessionPresent) {
  Serial.println("[MQTT] Connecté !");
  mqttClient.subscribe(MQTT_CMD_SYSTEM,     1);
  mqttClient.subscribe(MQTT_CMD_MODE,       1);
  mqttClient.subscribe(MQTT_CMD_OVERRIDE,   1);
  mqttClient.subscribe(MQTT_CMD_BRIGHTNESS, 1);
  mqttClient.subscribe(MQTT_CMD_COLOR,      1);
  mqttClient.subscribe(MQTT_CMD_SCHEDULE,   1);
  mqttPublishAll();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("[MQTT] Déconnecté — reconnexion dans 10s");
  if (WiFi.isConnected() && mqttReconnectTimer) xTimerStart(mqttReconnectTimer, 0);
}

void onMqttMessage(char* topic, char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
  String t = String(topic);
  String v = "";
  for (size_t i = 0; i < len; i++) v += (char)payload[i];
  v.trim();
  Serial.println("[MQTT] " + t + " : " + v);

  if (t == MQTT_CMD_SYSTEM) {
    cfg.systemActive = (v == "on");
    if (!cfg.systemActive) { overrideOn = false; pendingLeds = true; pendingLedsOn = false; pendingLedsSmooth = false; }
    saveConfig(); mqttPublishAll();
  } else if (t == MQTT_CMD_MODE) {
    cfg.modeOn = (v == "on");
    overrideOn = false;
    pendingLeds = true; pendingLedsOn = cfg.modeOn; pendingLedsSmooth = true;
    saveConfig(); mqttPublishAll();
  } else if (t == MQTT_CMD_OVERRIDE) {
    overrideOn = (v == "on");
    pendingLeds = true; pendingLedsOn = overrideOn; pendingLedsSmooth = true;
    if (!overrideOn) lightOffTime = 0;
    mqttPublishAll();
  } else if (t == MQTT_CMD_BRIGHTNESS) {
    int b = v.toInt();
    if (b >= 0 && b <= 100) { cfg.brightness = b; saveConfig(); }
    if (ledsOn) { pendingLeds = true; pendingLedsOn = true; pendingLedsSmooth = false; }
    mqttPublishAll();
  } else if (t == MQTT_CMD_COLOR) {
    if (v.length() == 7 && v[0] == '#') {
      cfg.red   = strtol(v.substring(1,3).c_str(), NULL, 16);
      cfg.green = strtol(v.substring(3,5).c_str(), NULL, 16);
      cfg.blue  = strtol(v.substring(5,7).c_str(), NULL, 16);
      saveConfig();
      if (ledsOn) { pendingLeds = true; pendingLedsOn = true; pendingLedsSmooth = false; }
      mqttPublishAll();
    }
  } else if (t == MQTT_CMD_SCHEDULE && cfg.scheduleMode == 1) {
    StaticJsonDocument<64> doc;
    if (deserializeJson(doc, v) == DeserializationError::Ok) {
      String start = doc["start"] | "";
      String end   = doc["end"]   | "";
      if (start.length() == 5) { cfg.startHour = start.substring(0,2).toInt(); cfg.startMin = start.substring(3,5).toInt(); }
      if (end.length() == 5)   { cfg.endHour   = end.substring(0,2).toInt();   cfg.endMin   = end.substring(3,5).toInt(); }
      saveConfig();
      Serial.printf("[MQTT] Plage HA: %02d:%02d→%02d:%02d\n", cfg.startHour, cfg.startMin, cfg.endHour, cfg.endMin);
    }
  }
}

void mqttPublishAll() {
  if (!cfg.mqttEnabled || !mqttClient.connected()) return;
  mqttClient.publish(MQTT_STATE_LEDS,       1, true, ledsOn           ? "on"  : "off");
  mqttClient.publish(MQTT_STATE_PIR,        1, true, pirState         ? "on"  : "off");
  mqttClient.publish(MQTT_STATE_MODE,       1, true, cfg.modeOn       ? "on"  : "auto");
  mqttClient.publish(MQTT_STATE_OVERRIDE,   1, true, overrideOn       ? "on"  : "off");
  mqttClient.publish(MQTT_STATE_SYSTEM,     1, true, cfg.systemActive  ? "on" : "off");
  mqttClient.publish(MQTT_STATE_BRIGHTNESS, 1, true, String(cfg.brightness).c_str());
  char colorBuf[8];
  snprintf(colorBuf, sizeof(colorBuf), "#%02x%02x%02x", cfg.red, cfg.green, cfg.blue);
  mqttClient.publish(MQTT_STATE_COLOR, 1, true, colorBuf);
  Serial.println("[MQTT] État publié");
}

void mqttConnect() {
  if (!cfg.mqttEnabled || cfg.mqttHost.length() == 0) return;
  Serial.print("[MQTT] Connexion à " + cfg.mqttHost + "...");
  mqttClient.setServer(cfg.mqttHost.c_str(), cfg.mqttPort);
  if (cfg.mqttUser.length() > 0) mqttClient.setCredentials(cfg.mqttUser.c_str(), cfg.mqttPass.c_str());
  mqttClient.setClientId(cfg.mqttId.c_str());
  mqttClient.connect();
}

// ─────────────────────────────────────────────
//  CONFIG
// ─────────────────────────────────────────────
void loadConfig() {
  if (!LittleFS.exists(CONFIG_FILE)) { saveConfig(); return; }
  File f = LittleFS.open(CONFIG_FILE, "r");
  if (!f) return;
  StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, f) == DeserializationError::Ok) {
    cfg.numLeds      = doc["numLeds"]      | cfg.numLeds;
    cfg.red          = doc["red"]          | cfg.red;
    cfg.green        = doc["green"]        | cfg.green;
    cfg.blue         = doc["blue"]         | cfg.blue;
    cfg.brightness   = doc["brightness"]   | cfg.brightness;
    cfg.duration     = doc["duration"]     | cfg.duration;
    cfg.fadeEffect   = doc["fadeEffect"]   | cfg.fadeEffect;
    cfg.startHour    = doc["startHour"]    | cfg.startHour;
    cfg.startMin     = doc["startMin"]     | cfg.startMin;
    cfg.endHour      = doc["endHour"]      | cfg.endHour;
    cfg.endMin       = doc["endMin"]       | cfg.endMin;
    cfg.pirDebounce  = doc["pirDebounce"]  | cfg.pirDebounce;
    cfg.systemActive = doc["systemActive"] | cfg.systemActive;
    cfg.modeOn       = doc["modeOn"]       | cfg.modeOn;
    cfg.langFR       = doc["langFR"]       | cfg.langFR;
    cfg.scheduleMode = doc["scheduleMode"] | cfg.scheduleMode;
    cfg.mqttEnabled  = doc["mqttEnabled"]  | cfg.mqttEnabled;
    cfg.mqttPort     = doc["mqttPort"]     | cfg.mqttPort;
    if (doc["timezone"].is<const char*>()) cfg.timezone = doc["timezone"].as<String>();
    if (doc["city"].is<const char*>())     cfg.city     = doc["city"].as<String>();
    if (doc["mqttHost"].is<const char*>()) cfg.mqttHost = doc["mqttHost"].as<String>();
    if (doc["mqttUser"].is<const char*>()) cfg.mqttUser = doc["mqttUser"].as<String>();
    if (doc["mqttPass"].is<const char*>()) cfg.mqttPass = doc["mqttPass"].as<String>();
    if (doc["mqttId"].is<const char*>())   cfg.mqttId   = doc["mqttId"].as<String>();
    if (cfg.mqttId.length() == 0)          cfg.mqttId   = "lumisetup";
  }
  f.close();
  Serial.println("[Config] Chargée");
}

void saveConfig() {
  File f = LittleFS.open(CONFIG_FILE, "w");
  if (!f) return;
  StaticJsonDocument<1024> doc;
  doc["numLeds"]      = cfg.numLeds;
  doc["red"]          = cfg.red;
  doc["green"]        = cfg.green;
  doc["blue"]         = cfg.blue;
  doc["brightness"]   = cfg.brightness;
  doc["duration"]     = cfg.duration;
  doc["fadeEffect"]   = cfg.fadeEffect;
  doc["startHour"]    = cfg.startHour;
  doc["startMin"]     = cfg.startMin;
  doc["endHour"]      = cfg.endHour;
  doc["endMin"]       = cfg.endMin;
  doc["pirDebounce"]  = cfg.pirDebounce;
  doc["systemActive"] = cfg.systemActive;
  doc["modeOn"]       = cfg.modeOn;
  doc["langFR"]       = cfg.langFR;
  doc["scheduleMode"] = cfg.scheduleMode;
  doc["timezone"]     = cfg.timezone;
  doc["city"]         = cfg.city;
  doc["mqttEnabled"]  = cfg.mqttEnabled;
  doc["mqttHost"]     = cfg.mqttHost;
  doc["mqttPort"]     = cfg.mqttPort;
  doc["mqttUser"]     = cfg.mqttUser;
  doc["mqttPass"]     = cfg.mqttPass;
  doc["mqttId"]       = cfg.mqttId;
  serializeJson(doc, f);
  f.close();
}

void loadWifi(String &ssid, String &pass) {
  ssid = ""; pass = "";
  if (!LittleFS.exists(WIFI_FILE)) { Serial.println("[WiFi] Pas de wifi.json"); return; }
  File f = LittleFS.open(WIFI_FILE, "r");
  if (!f) return;
  StaticJsonDocument<128> doc;
  if (deserializeJson(doc, f) == DeserializationError::Ok) {
    ssid = doc["ssid"] | ""; pass = doc["pass"] | "";
  }
  f.close();
  Serial.println("[WiFi] Credentials lus — SSID : " + ssid);
}

void saveWifi(const String &ssid, const String &pass) {
  File f = LittleFS.open(WIFI_FILE, "w");
  if (!f) return;
  StaticJsonDocument<128> doc;
  doc["ssid"] = ssid; doc["pass"] = pass;
  serializeJson(doc, f);
  f.flush(); f.close();
  Serial.println("[WiFi] Sauvegardé : " + ssid);
}

// ─────────────────────────────────────────────
//  JSON
// ─────────────────────────────────────────────
String buildStatusJson() {
  struct tm ti = getLocalTime_();
  char timeBuf[6];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", ti.tm_hour, ti.tm_min);
  StaticJsonDocument<384> doc;
  doc["systemActive"]  = cfg.systemActive;
  doc["modeOn"]        = cfg.modeOn;
  doc["overrideOn"]    = overrideOn;
  doc["ledsOn"]        = ledsOn;
  doc["pirState"]      = pirState;
  doc["inRange"]       = isInTimeRange();
  doc["time"]          = timeBuf;
  doc["langFR"]        = cfg.langFR;
  doc["mqttEnabled"]   = cfg.mqttEnabled;
  doc["mqttConnected"] = mqttClient.connected();
  String out; serializeJson(doc, out); return out;
}

String buildConfigJson() {
  StaticJsonDocument<1024> doc;
  doc["numLeds"]      = cfg.numLeds;
  doc["red"]          = cfg.red;
  doc["green"]        = cfg.green;
  doc["blue"]         = cfg.blue;
  doc["brightness"]   = cfg.brightness;
  doc["duration"]     = cfg.duration;
  doc["fadeEffect"]   = cfg.fadeEffect;
  doc["startHour"]    = cfg.startHour;
  doc["startMin"]     = cfg.startMin;
  doc["endHour"]      = cfg.endHour;
  doc["endMin"]       = cfg.endMin;
  doc["pirDebounce"]  = cfg.pirDebounce;
  doc["langFR"]       = cfg.langFR;
  doc["scheduleMode"] = cfg.scheduleMode;
  doc["timezone"]     = cfg.timezone;
  doc["city"]         = cfg.city;
  doc["mqttEnabled"]  = cfg.mqttEnabled;
  doc["mqttHost"]     = cfg.mqttHost;
  doc["mqttPort"]     = cfg.mqttPort;
  doc["mqttUser"]     = cfg.mqttUser;
  doc["mqttId"]       = cfg.mqttId;
  String out; serializeJson(doc, out); return out;
}

// ─────────────────────────────────────────────
//  PLAGE HORAIRE
// ─────────────────────────────────────────────
bool isInTimeRange() {
  struct tm ti = getLocalTime_();
  int now   = ti.tm_hour * 60 + ti.tm_min;
  int start = cfg.startHour * 60 + cfg.startMin;
  int end   = cfg.endHour   * 60 + cfg.endMin;
  if (start <= end) return (now >= start && now < end);
  return (now >= start || now < end);
}

// ─────────────────────────────────────────────
//  LEDS
// ─────────────────────────────────────────────
void applyLeds(bool on, bool smooth) {
  ledsOn = on;
  uint8_t target = map(cfg.brightness, 0, 100, 0, 255);
  if (on) {
    for (int i = 0; i < cfg.numLeds && i < MAX_LEDS; i++)
      leds[i] = CRGB(cfg.red, cfg.green, cfg.blue);
    if (smooth && cfg.fadeEffect) {
      for (int b = 0; b <= target; b += 4) { FastLED.setBrightness(b); FastLED.show(); delay(12); }
    }
    FastLED.setBrightness(target); FastLED.show();
  } else {
    if (smooth && cfg.fadeEffect) {
      for (int b = target; b >= 0; b -= 4) { FastLED.setBrightness(b); FastLED.show(); delay(12); }
    }
    FastLED.clear(); FastLED.setBrightness(0); FastLED.show();
  }
}

// ─────────────────────────────────────────────
//  PIR
// ─────────────────────────────────────────────
void handlePir() {
  bool motion = (digitalRead(PIR_PIN) == HIGH);
  unsigned long now = millis();
  if (motion && !pirState) {
    if (now - lastPirTime > (unsigned long)(cfg.pirDebounce * 1000UL)) {
      pirState = true; lastPirTime = now;
      Serial.println("[PIR] Mouvement !");
      if (isInTimeRange()) {
        Serial.println("[LED] Allumage !");
        applyLeds(true);
        lightOffTime = millis() + (unsigned long)(cfg.duration * 1000UL);
      }
    }
  } else if (!motion && pirState) {
    pirState = false;
  }
}

// ─────────────────────────────────────────────
//  PAGES HTML (fichiers séparés pour éviter le bug du preprocesseur Arduino)
// ─────────────────────────────────────────────
#include "portal_html.h"
#include "index_html.h"

// ─────────────────────────────────────────────
//  MODES DÉMARRAGE
// ─────────────────────────────────────────────
void startPortalMode() {
  portalMode = true;
  WiFi.mode(WIFI_AP);
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_NAME, AP_PASS);
  dnsServer.start(53, "*", apIP);
  setupPortalRoutes();
  server.begin();
  Serial.println("[Portal] Actif sur http://" AP_IP_STR);
}

void startNormalMode() {
  portalMode = false;
  applyTimezone();
  delay(2000); // laisser NTP se synchroniser
  struct tm ti;
  if (getLocalTime(&ti)) {
    Serial.printf("[NTP] %02d:%02d:%02d (%s)\n", ti.tm_hour, ti.tm_min, ti.tm_sec, cfg.timezone.c_str());
  }
  if (MDNS.begin(HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("[mDNS] http://" HOSTNAME ".local");
  }
  if (cfg.mqttEnabled && cfg.mqttHost.length() > 0) {
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(10000), pdFALSE,
      (void*)0, reinterpret_cast<TimerCallbackFunction_t>(mqttConnect));
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttConnect();
  }
  setupAppRoutes();
  server.begin();
  Serial.println("[HTTP] Démarré — IP : " + WiFi.localIP().toString());
  if (cfg.scheduleMode == 2) { fetchSunriseSunset(); lastSunUpdate = millis(); }
}

// ─────────────────────────────────────────────
//  ROUTES PORTAIL
// ─────────────────────────────────────────────
void setupPortalRoutes() {
  auto redir = [](AsyncWebServerRequest* req){ req->send_P(200, "text/html", PORTAL_HTML); };
  server.on("/", HTTP_GET, redir);
  server.on("/generate_204", HTTP_GET, redir);
  server.on("/gen_204", HTTP_GET, redir);
  server.on("/hotspot-detect.html", HTTP_GET, redir);
  server.on("/library/test/success.html", HTTP_GET, redir);
  server.on("/ncsi.txt", HTTP_GET, redir);
  server.on("/connecttest.txt", HTTP_GET, redir);
  server.on("/redirect", HTTP_GET, redir);
  server.onNotFound([](AsyncWebServerRequest* req){
    AsyncWebServerResponse* res = req->beginResponse_P(200, "text/html", PORTAL_HTML);
    res->addHeader("Cache-Control", "no-cache"); req->send(res);
  });
  server.on("/wifi/save", HTTP_POST, [](AsyncWebServerRequest* req){
    String ssid="", pass="";
    if(req->hasParam("ssid",true)) ssid=req->getParam("ssid",true)->value();
    if(req->hasParam("pass",true)) pass=req->getParam("pass",true)->value();
    saveWifi(ssid, pass);
    req->send(200,"text/plain","OK");
    delay(1000); ESP.restart();
  });
  server.on("/toggle/lang", HTTP_POST, [](AsyncWebServerRequest* req){
    cfg.langFR=!cfg.langFR; saveConfig();
    req->send(200,"text/plain","OK");
  });
}

// ─────────────────────────────────────────────
//  ROUTES APPLICATION
// ─────────────────────────────────────────────
void setupAppRoutes() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req){ req->send_P(200,"text/html",INDEX_HTML); });

  server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest* req){
    req->send(200,"application/json",
      "{\"name\":\"LumiSetup\",\"short_name\":\"LumiSetup\",\"start_url\":\"/\","
      "\"display\":\"standalone\",\"background_color\":\"#0f0f13\",\"theme_color\":\"#f5a623\","
      "\"icons\":[{\"src\":\"/icon.png\",\"sizes\":\"192x192\",\"type\":\"image/png\"}]}");
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest* req){ req->send(200,"application/json",buildStatusJson()); });
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest* req){ req->send(200,"application/json",buildConfigJson()); });

  server.on("/config", HTTP_POST, [](AsyncWebServerRequest* req){
    if(req->hasParam("numLeds",true))     cfg.numLeds     =req->getParam("numLeds",true)->value().toInt();
    if(req->hasParam("red",true))         cfg.red         =req->getParam("red",true)->value().toInt();
    if(req->hasParam("green",true))       cfg.green       =req->getParam("green",true)->value().toInt();
    if(req->hasParam("blue",true))        cfg.blue        =req->getParam("blue",true)->value().toInt();
    if(req->hasParam("brightness",true))  cfg.brightness  =req->getParam("brightness",true)->value().toInt();
    if(req->hasParam("duration",true))    cfg.duration    =req->getParam("duration",true)->value().toInt();
    if(req->hasParam("fadeEffect",true))  cfg.fadeEffect  =req->getParam("fadeEffect",true)->value()=="1";
    if(req->hasParam("startHour",true))   cfg.startHour   =req->getParam("startHour",true)->value().toInt();
    if(req->hasParam("startMin",true))    cfg.startMin    =req->getParam("startMin",true)->value().toInt();
    if(req->hasParam("endHour",true))     cfg.endHour     =req->getParam("endHour",true)->value().toInt();
    if(req->hasParam("endMin",true))      cfg.endMin      =req->getParam("endMin",true)->value().toInt();
    if(req->hasParam("pirDebounce",true)) cfg.pirDebounce =req->getParam("pirDebounce",true)->value().toInt();
    if(req->hasParam("scheduleMode",true))cfg.scheduleMode=req->getParam("scheduleMode",true)->value().toInt();
    if(req->hasParam("city",true))        cfg.city        =req->getParam("city",true)->value();
    if(req->hasParam("timezone",true)){   cfg.timezone    =req->getParam("timezone",true)->value(); applyTimezone(); }
    if(req->hasParam("mqttEnabled",true)) cfg.mqttEnabled =req->getParam("mqttEnabled",true)->value()=="1";
    if(req->hasParam("mqttHost",true))    cfg.mqttHost    =req->getParam("mqttHost",true)->value();
    if(req->hasParam("mqttPort",true))    cfg.mqttPort    =req->getParam("mqttPort",true)->value().toInt();
    if(req->hasParam("mqttUser",true))    cfg.mqttUser    =req->getParam("mqttUser",true)->value();
    if(req->hasParam("mqttPass",true))    cfg.mqttPass    =req->getParam("mqttPass",true)->value();
    if(req->hasParam("mqttId",true))      cfg.mqttId      =req->getParam("mqttId",true)->value();
    saveConfig();
    if(cfg.scheduleMode==2){ fetchSunriseSunset(); lastSunUpdate=millis(); }
    req->send(200,"application/json","{\"ok\":true}");
  });

  server.on("/toggle/system", HTTP_POST, [](AsyncWebServerRequest* req){
    cfg.systemActive=!cfg.systemActive;
    if(!cfg.systemActive){overrideOn=false;pendingLeds=true;pendingLedsOn=false;pendingLedsSmooth=false;}
    saveConfig(); mqttPublishAll();
    req->send(200,"application/json",buildStatusJson());
  });

  server.on("/toggle/mode", HTTP_POST, [](AsyncWebServerRequest* req){
    cfg.modeOn=!cfg.modeOn; overrideOn=false;
    pendingLeds=true; pendingLedsOn=cfg.modeOn; pendingLedsSmooth=true;
    saveConfig(); mqttPublishAll();
    req->send(200,"application/json",buildStatusJson());
  });

  server.on("/toggle/override", HTTP_POST, [](AsyncWebServerRequest* req){
    overrideOn=!overrideOn;
    pendingLeds=true; pendingLedsOn=overrideOn; pendingLedsSmooth=true;
    if(!overrideOn) lightOffTime=0;
    mqttPublishAll();
    req->send(200,"application/json",buildStatusJson());
  });

  server.on("/toggle/lang", HTTP_POST, [](AsyncWebServerRequest* req){
    cfg.langFR=!cfg.langFR; saveConfig();
    req->send(200,"application/json",buildStatusJson());
  });

  server.on("/sun/test", HTTP_GET, [](AsyncWebServerRequest* req){
    String city=cfg.city;
    if(req->hasParam("city")) city=req->getParam("city")->value();
    HTTPClient http;
    http.begin("https://wttr.in/"+city+"?format=j1");
    http.setTimeout(8000);
    int code=http.GET();
    if(code!=200){http.end();req->send(200,"application/json","{\"ok\":false,\"msg\":\"Ville introuvable\"}");return;}
    String payload=http.getString(); http.end();
    StaticJsonDocument<4096> doc;
    if(deserializeJson(doc,payload)!=DeserializationError::Ok){req->send(200,"application/json","{\"ok\":false,\"msg\":\"Erreur JSON\"}");return;}
    const char* sr=doc["weather"][0]["astronomy"][0]["sunrise"];
    const char* ss=doc["weather"][0]["astronomy"][0]["sunset"];
    if(!sr||!ss){req->send(200,"application/json","{\"ok\":false,\"msg\":\"Données manquantes\"}");return;}
    req->send(200,"application/json","{\"ok\":true,\"sunrise\":\""+String(sr)+"\",\"sunset\":\""+String(ss)+"\"}");
  });

  server.on("/mqtt/test", HTTP_POST, [](AsyncWebServerRequest* req){
    if(!cfg.mqttEnabled||cfg.mqttHost.length()==0){req->send(200,"application/json","{\"ok\":false,\"msg\":\"MQTT non configuré\"}");return;}
    if(mqttClient.connected()){mqttPublishAll();req->send(200,"application/json","{\"ok\":true,\"msg\":\"Connecté — état publié\"}");}
    else{mqttConnect();req->send(200,"application/json","{\"ok\":true,\"msg\":\"Tentative de connexion...\"}");}
  });

  server.on("/wifi/reset", HTTP_POST, [](AsyncWebServerRequest* req){
    req->send(200,"application/json","{\"ok\":true}");
    delay(500); LittleFS.remove(WIFI_FILE); ESP.restart();
  });

  server.onNotFound([](AsyncWebServerRequest* req){ req->send(404,"text/plain","Non trouvé"); });
}

// ─────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n[BOOT] LumiSetup ESP32 démarrage...");

  if(!LittleFS.begin(true)){Serial.println("[FS] Erreur");return;}
  loadConfig();

  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, MAX_LEDS);
  FastLED.clear(); FastLED.show();

  pinMode(PIR_PIN, INPUT);
  delay(200);

  String ssid, pass;
  loadWifi(ssid, pass);

  if(ssid.length()>0){
    WiFi.setHostname(HOSTNAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.println("[WiFi] Connexion à : "+ssid);
    unsigned long t=millis();
    while(WiFi.status()!=WL_CONNECTED&&millis()-t<20000){delay(500);Serial.print(".");}
    Serial.println();
  }

  if(WiFi.status()==WL_CONNECTED){
    Serial.println("[WiFi] Connecté — IP : "+WiFi.localIP().toString());
    startNormalMode();
  } else {
    Serial.println("[WiFi] Échec — portail "+String(AP_NAME));
    startPortalMode();
  }
}

// ─────────────────────────────────────────────
//  LOOP
// ─────────────────────────────────────────────
void loop() {
  if(portalMode){dnsServer.processNextRequest();delay(10);return;}

  if(pendingLeds){pendingLeds=false;applyLeds(pendingLedsOn,pendingLedsSmooth);}

  // Mise à jour soleil standalone toutes les 24h
  if(cfg.scheduleMode==2&&millis()-lastSunUpdate>SUN_UPDATE_INTERVAL){
    fetchSunriseSunset(); lastSunUpdate=millis();
  }

  // Publier MQTT si état changé
  if(cfg.mqttEnabled&&mqttClient.connected()){
    if(ledsOn!=lastLedsOn||pirState!=lastPirState){
      lastLedsOn=ledsOn; lastPirState=pirState;
      mqttPublishAll();
    }
  }

  if(!cfg.systemActive){if(ledsOn)applyLeds(false,false);delay(200);return;}
  if(cfg.modeOn){if(!ledsOn)applyLeds(true);delay(100);return;}
  if(overrideOn){if(!ledsOn)applyLeds(true);delay(100);return;}

  handlePir();

  if(ledsOn&&lightOffTime>0&&millis()>=lightOffTime){
    applyLeds(false); lightOffTime=0;
    Serial.println("[LED] Extinction automatique");
    mqttPublishAll();
  }

  delay(50);
}
