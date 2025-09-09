#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

RTC_DS1307 rtc;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DateTime baseTime;
unsigned long baseMillis;
float speedFactor = 1.0; // 1/0.75 = 1.333... (i.e., "seconds" move 33% faster)

void setup() {
  Serial.begin(19200);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(2025, 9, 9, 20, 9, 0)); // Set your desired manual time

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RTC Clock Ready (simulated speed)");
  display.display();

  baseTime = rtc.now();
  baseMillis = millis();
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long nowMillis = millis();

  // Display update every 100 ms; display time runs faster than RTC
  if (nowMillis - lastUpdate >= 100) {
    lastUpdate = nowMillis;

    // Simulate faster seconds
    unsigned long elapsed = nowMillis - baseMillis;
    unsigned long adjustedElapsed = (unsigned long)(elapsed * speedFactor);
    DateTime displayTime = baseTime + TimeSpan(adjustedElapsed / 1000);

    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d",
             displayTime.year(), displayTime.month(), displayTime.day(),
             displayTime.hour(), displayTime.minute(), displayTime.second());

    Serial.print("Simulated RTC time: ");
    Serial.println(timeStr);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("RTC Clock Ready (sim)");
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println(timeStr);
    display.display();
  }
}
