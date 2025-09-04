# Water_Flow_Rate
Digital water meter and billing system with Arduino: live OLED, RTC, simulation &amp; Bluetooth logging.
# 💧 Digital Water Flow Meter and Billing System

## Overview

This project is a comprehensive **digital water flow meter** with **automatic billing**, built for both **real hardware** and **simulation (Wokwi)**.  
It uses an **Arduino Nano**, **YF-S201 flow sensor**, **SSD1306 OLED display**, and a **DS3231 RTC**. The system measures real-time flow, calculates total volume, and computes cost per liter, showing everything on the OLED. 

You can try the logic out for yourself with the provided [Wokwi simulation](https://wokwi.com/projects/439189480588692481).

---

## 📦 Features

- **Hardware implementation** (fully tested and robust, with Bluetooth export and bill cycle resets)
- **Online simulation** (just run in Wokwi!)
- Live **flow rate** (liters/min), **total volume** (liters), and **billing** (in INR)
- Real-time **time display** using DS3231 RTC (with instructions for serial)
- Set **cost per liter** in code, fits any local water rate
- Clean OLED display with intuitive layout
- Easy wiring; robust interrupt-driven pulse reading for accurate measurement

---

## 🛠️ Hardware Requirements

| Component          | Quantity | Details                  |
| ------------------ | -------- | ------------------------ |
| Arduino Nano       | 1        | Or Uno, Pro Mini, etc    |
| YF-S201 Flow Sensor| 1        | Water flow measurement   |
| SSD1306 OLED (I2C) | 1        | 128x64                   |
| DS3231 RTC Module  | 1        | Real-Time Clock          |
| HC-05/06 Bluetooth | 1        | For wireless logging     |
| Wires/Breadboard   | -        | As needed                |
| Battery (for RTC)  | 1        | Coin cell for time backup|

---

## 🧑‍💻 Hardware Arduino Code

[See full code above or in `/hardware/arduino-final.ino` in this repo.], which supports:
- YF-S201 flow measurement (pin 2)
- OLED output (A4/A5 for I2C)
- DS3231 RTC time/date
- Serial and Bluetooth output 
- Configurable cost per liter
- Billing cycle reset


---

## 🌐 Simulation on Wokwi

- Direct Link: [Wokwi Simulation Project](https://wokwi.com/projects/439189480588692481)

**How to use:**
1. Click the link and hit the green "Start" button.
2. See the OLED virtual display showing time, flow, volume, and bill as pulses (simulated flow) are sent.
3. Try changing `cost_per_liter` or triggering pulses in Wokwi for live updates.

**Simulation code starter:**
*(Directly as in the provided Wokwi project)*

---

## 📊 Example Output

- **OLED**:  
  - Time (14:06:45)
  - Flow: 19.36 L/min
  - Vol: 20.06 L
  - Bill: INR 125.35

- **Serial Monitor/Bluetooth**:
- 19.36 L/min | Total Vol: 19.39 L | Bill: INR 125.35
- ## 📂 Folders

- `/hardware/` — final tested code and wiring schematic
- `/simulation/` — code for use in Wokwi (matches the provided project)

---

## 📜 License

MIT License

---

**Project by**:  
   - Likith Kumar B M - 2460812  
   - Jeni Riya Lobo - 2460810  
   - Suryan E - 2460815  
  





