/*
 * webserver_relay.ino — کنترل ۴ رله Esp32 IX4 از طریق مرورگر (هم‌خانواده با دموی رسمی ایرانیک)
 * نسخه بدون وابستگی خارجی (WebServer داخلی core ESP32) — ساده، پایدار، برای تست و آموزش.
 * نسخه رسمی_fabric با ESPAsyncWebServer + WebSockets:
 *   https://server4.eca.ir/eshop/000/IRANICBoard/ESP32/IRANICBoard_ESP32_WebServer_3011027068.rar
 * استفاده:
 *   1) ssid/pass را پر کنید (یا اگر شبکه یافت نشد، برد خودش AP با نام IX4-RELAY می‌سازد، IP=192.168.4.1)
 *   2) آپلود کنید، سریال ۱۱۵۲۰۰ → IP چاپ می‌شود
 *   3) در همان شبکه IP را باز کنید
 */

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_SSID";
const char* pass = "YOUR_PASSWORD";

const uint8_t RELAY_PINS[4] = {4, 2, 13, 12};
WebServer server(80);

bool st[4] = {0,0,0,0};

String page() {
  String s = F("<!DOCTYPE html><html lang='fa' dir='rtl'><head><meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Esp32 IX4 Relay Board</title><style>"
    "body{font-family:Tahoma;background:#0d1622;color:#eee;text-align:center;padding:20px}"
    "h2{color:#7dd6a8}.r{background:#16233a;border:1px solid #26375a;border-radius:12px;"
    "margin:10px auto;padding:16px;max-width:320px;display:flex;justify-content:space-between;align-items:center}"
    ".on{color:#2fd65f}.off{color:#f2453b}a{color:#fff;background:#1d6fd8;padding:8px 18px;border-radius:8px;text-decoration:none}"
    "a.t{background:#f29d11}</style></head><body><h2>برد Esp32 IX4 ‏۴ رله</h2>");
  for (int i = 0; i < 4; i++) {
    s += "<div class='r'><b>رله " + String(i+1) + " (GPIO" + String(RELAY_PINS[i]) + ")</b>"
      + "<span class='" + (st[i] ? "on" : "off") + "'>" + (st[i] ? "روشن" : "خاموش") + "</span>"
      + "<span><a href='/r/" + String(i) + "/1'>ON</a> <a href='/r/" + String(i) + "/0'>OFF</a> "
      + "<a class='t' href='/r/" + String(i) + "/t'>Toggle</a></span></div>";
  }
  s += F("<p style='color:#8fa5bd;font-size:12px'>iranic IX4 — RL1..4 ← GPIO4/2/13/12</p></body></html>");
  return s;
}

void handleRoot() { server.send(200, "text/html", page()); }
void handleRelay() {
  // /r/<idx>/<0|1|t>
  String p = server.uri(); int a = p.indexOf('/', 3), i = p.substring(3, a).toInt();
  String cmd = p.substring(a + 1);
  if (i >= 0 && i < 4) {
    if (cmd == "1") st[i] = 1; else if (cmd == "0") st[i] = 0; else if (cmd == "t") st[i] = !st[i];
    digitalWrite(RELAY_PINS[i], st[i]);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}
void handleApi() {
  // /api?r=0..3&act=0|1|t  → JSON state of all
  if (server.hasArg("r")) {
    int i = server.arg("r").toInt();
    String act = server.hasArg("act") ? server.arg("act") : "t";
    if (i >= 0 && i < 4) {
      if (act == "1") st[i] = 1; else if (act == "0") st[i] = 0; else st[i] = !st[i];
      digitalWrite(RELAY_PINS[i], st[i]);
    }
  }
  String j = "{\"state\":[" + String(st[0]) + "," + String(st[1]) + "," + String(st[2]) + "," + String(st[3]) + "]}";
  server.send(200, "application/json", j);
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) { pinMode(RELAY_PINS[i], OUTPUT); digitalWrite(RELAY_PINS[i], LOW); }
  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  for (int t = 0; t < 40 && WiFi.status() != WL_CONNECTED; t++) { delay(250); Serial.print('.'); }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nConnected! IP = %s\n", WiFi.localIP().toString().c_str());
  } else {
    WiFi.softAP("IX4-RELAY");
    Serial.printf("\nAP mode. IP = %s\n", WiFi.softAPIP().toString().c_str());
  }
  server.on("/", handleRoot);
  server.on("/api", handleApi);
  server.begin();
  Serial.println("Web server started.");
}

void loop() { server.handleClient(); }
