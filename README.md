# Wall Following Robot — Team 11

## Overview
Autonomous wall-following robot built on AVR ATmega328P.
Follows walls, detects 90° turns (L/R), counts them, sends report via Bluetooth.

---

## Hardware
| Component | Details |
|---|---|
| MCU | Arduino Uno R3 — AVR ATmega328P @ 16 MHz |
| IR Sensors | 2x Sharp GP2Y0A21 (Left & Right sides) |
| Ultrasonic | HC-SR04 (Front) |
| Motors | GA25-370 DC with encoders |
| Motor Driver | TB6612FNG |
| Bluetooth | HC-05 |

---

## Pin Assignment (LOCKED — do not change without team discussion)

| Signal | AVR Pin | Notes |
|---|---|---|
| IR Left | PC0 (ADC0) | Analog input |
| IR Right | PC1 (ADC1) | Analog input |
| Ultrasonic TRIG | PD4 | Digital output |
| Ultrasonic ECHO | PB0 (ICP1) | Timer1 input capture |
| Motor L PWM | PD5 (OC0B) | Timer0 |
| Motor R PWM | PD6 (OC0A) | Timer0 |
| Motor L IN1 | PC2 | Direction |
| Motor L IN2 | PC3 | Direction |
| Motor R IN1 | PC4 | Direction |
| Motor R IN2 | PC5 | Direction |
| TB6612 STBY | PB4 | Active HIGH = enabled |
| Encoder Left | PD2 (INT0) | External interrupt |
| Encoder Right | PD3 (INT1) | External interrupt |
| UART TX | PD1 | → HC-05 RX |
| UART RX | PD0 | ← HC-05 TX |

---

## Timer Assignment (LOCKED)
| Timer | Role |
|---|---|
| Timer0 | Motor PWM (OC0A, OC0B) |
| Timer1 | Ultrasonic echo capture (ICP1) |
| Timer2 | 1ms system tick (millis) |

---

## Project Structure
firmware/
config/   → pins.h  (pin + timer definitions — read before writing any driver)
inc/      → header files (one per module)
src/      → source files (one per module)
docs/       → reports, FSM diagrams
hardware/   → schematics
test/       → per-module test sketches


---

## Development Rules
- **Branch:** all work goes on `main`
- **Commits:** small and focused — one logical change per commit
- **Push:** after every 2-3 commits
- **NEVER** use Arduino APIs (`digitalWrite`, `analogRead`, etc.)
- **NEVER** use blocking delays (`delay()`, `_delay_ms()` in control loop)
- **ALWAYS** read `firmware/config/pins.h` before writing a driver
- **ALWAYS** test your module before touching FSM

---

## Module Interface (agreed function names — do not rename)
```c
/* IR Sensor */
uint16_t IR_GetLeftDistance(void);
uint16_t IR_GetRightDistance(void);

/* Ultrasonic */
uint16_t Ultrasonic_GetDistance(void);

/* Motor */
void Motor_SetSpeed(int left, int right);
void Motor_Stop(void);

/* Encoder */
int32_t Encoder_GetLeft(void);
int32_t Encoder_GetRight(void);
```

---

## Build Setup
- IDE: VS Code + PlatformIO extension
- Clone the repo, open the folder in VS Code
- PlatformIO auto-detects `platformio.ini`
- Create `.vscode/c_cpp_properties.json` for IntelliSense (see setup notes)
- Build: `pio run` — Upload: `pio run --target upload`

---

## Progress
- [x] Project structure
- [x] Pin assignment locked (`firmware/config/pins.h`)
- [x] PlatformIO configured (`platformio.ini`)
- [ ] ADC driver
- [ ] IR sensor driver
- [ ] Ultrasonic driver
- [ ] Motor driver
- [ ] Encoder driver
- [ ] UART driver
- [ ] FSM implementation