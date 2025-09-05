#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

volatile int flow_frequency = 0; // Measures flow sensor pulses
float vol = 0, l_minute;
float cost_per_liter = 6.25;   // <--- Set your billing price here (INR per liter)
float billing_amount = 0;
const unsigned char flowsensor = 2; // Sensor input pin
unsigned long currentTime;
unsigned long cloopTime;

void flow() { flow_frequency++; }

void setup() {
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional internal pull-up
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) ;
  }

  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    for (;;) ;
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Water Flow Meter"));
  display.display();

  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING);

  currentTime = millis();
  cloopTime = currentTime;
}

void loop() {
  currentTime = millis();
  // Every second, calculate and print liters per minute, volume and bill
  if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime;
    display.clearDisplay();
    display.setCursor(0, 0);

    // Get time from RTC
    DateTime now = rtc.now();
    if (now.hour() < 10) display.print('0');
    display.print(now.hour());
    display.print(':');
    if (now.minute() < 10) display.print('0');
    display.print(now.minute());
    display.print(':');
    if (now.second() < 10) display.print('0');
    display.print(now.second());

    display.setCursor(0, 16);

    if (flow_frequency != 0) {
      l_minute = (flow_frequency / 7.5); // flow rate in L/min

      display.print(F("Rate: "));
      display.print(l_minute, 2);
      display.println(F(" L/M"));

      l_minute = l_minute / 60.0; // convert to L/sec for volume summing
      vol += l_minute;

      display.print(F("Vol: "));
      display.print(vol, 2);
      display.println(F(" L"));

      // Calculate bill
      billing_amount = vol * cost_per_liter;
      display.print(F("Bill: INR "));
      display.print(billing_amount, 2);

      Serial.print(l_minute, 3);
      Serial.print(F(" L/Sec, Vol: "));
      Serial.print(vol, 2);
      Serial.print(F(" L, Bill: INR "));
      Serial.println(billing_amount, 2);
    } else {
      display.print(F("Rate: 0 L/M"));
      display.setCursor(0, 32);
      display.print(F("Vol: "));
      display.print(vol, 2);
      display.println(F(" L"));
      display.print(F("Bill: INR "));
      display.print(billing_amount, 2);

      Serial.println(F("flow rate = 0"));
    }

    display.display();
    flow_frequency = 0;
  }
}
