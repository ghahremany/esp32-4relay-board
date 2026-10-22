/*
 * pin_finder.ino — راستی‌آزمایی نگاشت رله‌ها روی Esp32 IX4 Relay Board (iranic)
 * از روی صفحه شماره‌دار رسمی و سیلک PCB: RELAY1..4 ← GPIO 4/2/13/12
 * خروجی: سریال ۱۱۵۲۰۰ + کلیک کانال‌ها یکی‌بی‌یک؛ LED هر کانال نیز با آن خاموش/روشن می‌شود.
 * توجه: GPIO2 و GPIO12 پایه‌های استرپینگ بوت هستند؛ این اسکچ برای کار بعد از بوت طراحی شده است.
 */

const uint8_t RELAY_PINS[4] = {4, 2, 13, 12};   // R1=4, R2=2, R3=13, R4=12 (سیلک: D04/D02/D13/D12)
const char* RELAY_NAME[4] = {"RELAY1", "RELAY2", "RELAY3", "RELAY4"};

void setup() {
  Serial.begin(115200);
  delay(400);
  Serial.println(F("\n== Esp32 IX4 pin finder =="));
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);     // درایور NPN فعال‌بالا → رله از اول خاموش
  }
  Serial.println(F("Sequence: RELAY1..RELAY4, 1s ON each. Watch click + channel LED."));
}

void loop() {
  for (uint8_t i = 0; i < 4; i++) {
    Serial.printf("%s (GPIO%d) ON\n", RELAY_NAME[i], RELAY_PINS[i]);
    digitalWrite(RELAY_PINS[i], HIGH);
    delay(1000);
    digitalWrite(RELAY_PINS[i], LOW);
    Serial.printf("%s (GPIO%d) OFF\n", RELAY_NAME[i], RELAY_PINS[i]);
    delay(400);
  }
  Serial.println(F("--- repeat ---"));
  delay(1500);
}
