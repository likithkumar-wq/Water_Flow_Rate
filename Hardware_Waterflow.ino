#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define FLOWSENSOR_PIN 2      // YF-S201 output to D2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;
SoftwareSerial BTSerial(10, 11); // RX, TX pins for HC-05/06

volatile uint16_t flow_frequency = 0; // Pulse counter
float flowRate = 0.0;                 // L/min
float totalVolume = 0.0;              // Liters
float costPerLiter = 6.25;            // ₹ per liter (adjust as needed)
float billingAmount = 0.0;

unsigned long currentTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long lastBluetoothTransmit = 0;

// For averaging/reporting each minute
uint16_t sumFlowRate = 0;
uint8_t samplesCollected = 0;

// 30 days interval (milliseconds)
const unsigned long billingResetInterval = 30UL * 24UL * 60UL * 60UL * 1000UL;
unsigned long lastBillingReset = 0;

void flow() { flow_frequency++; }  // Interrupt routine

void setup() {
  pinMode(FLOWSENSOR_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  BTSerial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { while (1); }
  if (!rtc.begin()) { while (1); }
  
  if (rtc.lostPower()) {
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Commented to allow serial time set
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Water Meter Ready");
  display.display();
  delay(2000);
  display.clearDisplay();

  attachInterrupt(digitalPinToInterrupt(FLOWSENSOR_PIN), flow, RISING);
  lastUpdateTime = millis();
  lastBluetoothTransmit = millis();
  lastBillingReset = millis();

  Serial.println(F("Send RTC time as: T:YYYY-MM-DD HH:MM:SS"));
}

void loop() {
  // Check for RTC time update command via Serial
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.startsWith("T:")) {
      input.remove(0, 2);
      if (input.length() >= 19) {
        int y = input.substring(0,4).toInt();
        int mo = input.substring(5,7).toInt();
        int d = input.substring(8,10).toInt();
        int h = input.substring(11,13).toInt();
        int mi = input.substring(14,16).toInt();
        int s = input.substring(17,19).toInt();
        rtc.adjust(DateTime(y, mo, d, h, mi, s));
        Serial.println(F("RTC updated!"));
      } else {
        Serial.println(F("Invalid RTC format! Use T:YYYY-MM-DD HH:MM:SS"));
      }
    }
  }

  currentTime = millis();
  DateTime now = rtc.now();

  // Update every second: flow, volume, billing
  if (currentTime - lastUpdateTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOWSENSOR_PIN));
    
    flowRate = flow_frequency / 7.5; // calibrated L/min
    float flowRatePerSecond = flowRate / 60.0;
    totalVolume += flowRatePerSecond;
    billingAmount = totalVolume * costPerLiter;

    sumFlowRate += flowRate;
    samplesCollected++;

    flow_frequency = 0;
    lastUpdateTime = currentTime;

    // Billing reset every 30 days
    if (currentTime - lastBillingReset >= billingResetInterval) {
      totalVolume = 0;
      billingAmount = 0;
      lastBillingReset = currentTime;
      BTSerial.println("Billing cycle reset.");
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Billing Reset.");
      display.display();
      delay(2000);
    }

    // OLED Display
    display.clearDisplay();
    display.setCursor(0, 0);
    if (now.hour() < 10) display.print('0');
    display.print(now.hour());
    display.print(':');
    if (now.minute() < 10) display.print('0');
    display.print(now.minute());
    display.print(':');
    if (now.second() < 10) display.print('0');
    display.print(now.second());

    display.setCursor(0, 16);
    display.print("Flow: ");
    display.print(flowRate, 2);
    display.println(" L/min");

    display.setCursor(0, 32);
    display.print("Vol: ");
    display.print(totalVolume, 2);
    display.println(" L");

    display.setCursor(0, 48);
    display.print("Bill: INR ");
    display.print(billingAmount, 2);

    display.display();
    attachInterrupt(digitalPinToInterrupt(FLOWSENSOR_PIN), flow, RISING);
  }

  // Bluetooth data every 1 minute
  if (currentTime - lastBluetoothTransmit >= 60000 && samplesCollected > 0) {
    float avgFlowRate = sumFlowRate / float(samplesCollected);
    BTSerial.print("Time: ");
    if (now.hour() < 10) BTSerial.print('0');
    BTSerial.print(now.hour()); BTSerial.print(':');
    if (now.minute() < 10) BTSerial.print('0');
    BTSerial.print(now.minute());
    BTSerial.print(" | Avg Flow: ");
    BTSerial.print(avgFlowRate, 2);
    BTSerial.print(" L/min | Total Vol: ");
    BTSerial.print(totalVolume, 2);
    BTSerial.print(" L | Bill: INR ");
    BTSerial.println(billingAmount, 2);

    Serial.print("Time: ");
    if (now.hour() < 10) Serial.print('0');
    Serial.print(now.hour()); Serial.print(':');
    if (now.minute() < 10) Serial.print('0');
    Serial.print(now.minute());
    Serial.print(" | Avg Flow: ");
    Serial.print(avgFlowRate, 2);
    Serial.print(" L/min | Total Vol: ");
    Serial.print(totalVolume, 2);
    Serial.print(" L | Bill: INR ");
    Serial.println(billingAmount, 2);

    sumFlowRate = 0;
    samplesCollected = 0;
    lastBluetoothTransmit = currentTime;
  }
}
