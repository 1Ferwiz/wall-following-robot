# Wall Following Robot (Embedded Systems Project)

## 📌 Overview

This project implements an autonomous wall-following robot that:

* Follows walls inside a track
* Detects 90° turns (Left / Right)
* Counts and stores turns
* Sends results to PC via Bluetooth

---

## ⚙️ Hardware Used

* AVR Microcontroller
* 2x Sharp IR Sensors (Left & Right)
* HC-SR04 Ultrasonic Sensor (Front)
* TB6612FNG Motor Driver
* GA25-370 DC Motors with Encoders
* HC-05 Bluetooth Module

---

## 🧠 System Design

* Finite State Machine (FSM)
* Non-blocking real-time control
* PID wall-following controller
* Interrupt-based encoder reading
* UART communication (Bluetooth)

---

## 📁 Project Structure

* `firmware/inc` → Header files
* `firmware/src` → Source files
* `firmware/config` → Pin + timer configuration
* `docs/` → Reports, FSM diagrams
* `hardware/` → Schematics

---

## 🚨 Rules (VERY IMPORTANT)

* ❌ No blocking delays (no delay loops)
* ❌ Do NOT write everything in main.c
* ✅ Each module must be separate
* ✅ Use timers + interrupts
* ✅ Follow FSM logic

---

## 👥 Team Responsibilities

Each member works on ONE module only:

* IR Sensors → `ir_sensor.c`
* Ultrasonic → `ultrasonic.c`
* Motor Driver (TB6612FNG) → `motor.c`
* Encoders → `encoder.c`
* FSM + Main Control → `fsm.c`, `main.c`

---

## 🌿 Git Workflow

1. Clone repo:

   ```
   git clone <repo-link>
   ```

2. Create branch:

   ```
   git checkout -b feature/module-name
   ```

3. Work → then commit:

   ```
   git add .
   git commit -m "your work"
   git push origin feature/module-name
   ```

4. Open Pull Request on GitHub

---

## ⚠️ Notes

* Keep code clean and modular
* Test your module independently
* Do NOT break other modules
* Communicate before changing shared files

---
