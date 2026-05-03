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
| Motors | GA25-370 DC with encoders (6-wire) |
| Motor Driver | TB6612FNG breakout |
| Bluetooth | HC-05 breakout |

---

## Pin Assignment (LOCKED — do not change without team discussion)

| Signal | Arduino Pin | AVR Pin | Notes |
|---|---|---|---|
| IR Left | A0 | PC0 (ADC0) | Analog input |
| IR Right | A1 | PC1 (ADC1) | Analog input |
| Ultrasonic TRIG | D4 | PD4 | Digital output |
| Ultrasonic ECHO | D8 | PB0 (ICP1) | Timer1 input capture |
| Motor L PWM | D5 | PD5 (OC0B) | Timer0 — Left motor |
| Motor R PWM | D6 | PD6 (OC0A) | Timer0 — Right motor |
| Motor L IN1 | A2 | PC2 | Direction |
| Motor L IN2 | A3 | PC3 | Direction |
| Motor R IN1 | A4 | PC4 | Direction |
| Motor R IN2 | A5 | PC5 | Direction |
| TB6612 STBY | D12 | PB4 | HIGH = enabled |
| Encoder Left A | D2 | PD2 (INT0) | External interrupt |
| Encoder Right A | D3 | PD3 (INT1) | External interrupt |
| Encoder Left B | D9 | PB1 | Future use |
| Encoder Right B | D10 | PB2 | Future use |
| UART TX | D1 | PD1 | → HC-05 RX |
| UART RX | D0 | PD0 | ← HC-05 TX |

---

## Motor Encoder Wire Colors (GA25-370)
| Wire Color | Function | Connect To |
|---|---|---|
| Red | Motor M+ | TB6612 AO1 / BO1 |
| Black | Motor M- | TB6612 AO2 / BO2 |
| Yellow | Encoder VCC | Arduino 5V |
| White | Encoder GND | Arduino GND |
| Green | Encoder A | D2 (left) / D3 (right) |
| Blue | Encoder B | D9 (left) / D10 (right) |

---

## Timer Assignment (LOCKED)
| Timer | Role |
|---|---|
| Timer0 | Motor PWM (OC0B=D5 Left, OC0A=D6 Right) |
| Timer1 | Ultrasonic echo capture (ICP1=D8) |
| Timer2 | 1ms system tick (millis) |

---

## Project Structure
```
firmware/
  config/   → pins.h  (pin + timer definitions — read before writing any driver)
  inc/      → header files (one per module)
  src/      → source files (one per module)
docs/       → reports, FSM diagrams
hardware/   → schematics
test/       → per-module test sketches
```

---

## Module Status
| Module | File | Owner | Status |
|---|---|---|---|
| ADC | adc.c | Person 1 | ✅ Done |
| IR Sensor | ir_sensor.c | Person 1 | ✅ Done |
| Ultrasonic | ultrasonic.c | Person 1 | ✅ Done |
| Motor | motor.c | Person 2 | ✅ Done |
| Encoder | encoder.c | Person 3 | 🔄 In progress |
| UART | uart.c | Person 4 | 🔄 In progress |
| FSM | fsm.c | All | ⏳ Waiting |
| Main loop | main.c | All | ⏳ Waiting |

---

## Module Interfaces (agreed — do not rename)
```c
/* IR Sensor */
void     IR_Init(void);
uint16_t IR_GetLeftDistance(void);   // returns mm
uint16_t IR_GetRightDistance(void);  // returns mm

/* Ultrasonic */
void     Ultrasonic_Init(void);
void     Ultrasonic_Trigger(void);      // call every 20ms
uint16_t Ultrasonic_GetDistance(void);  // returns mm, non-blocking

/* Motor — lifecycle */
void motor_init(void);          // call once at startup
void motor_enable(void);        // STBY HIGH — TB6612 active
void motor_disable(void);       // STBY LOW  — TB6612 standby

/* Motor — FSM motion commands */
void motor_forward(uint8_t speed);      // both wheels forward
void motor_backward(uint8_t speed);     // both wheels backward
void motor_stop(void);                  // coast stop
void motor_brake(void);                 // active brake — use before turns
void motor_turn_left(uint8_t speed);    // in-place left  rotation
void motor_turn_right(uint8_t speed);   // in-place right rotation

/* Motor — PID wall-following */
void motor_set_differential(int16_t left_pwm, int16_t right_pwm); // signed -255 to +255

/* Motor — individual control */
void motor_left_set(uint8_t speed, uint8_t dir);
void motor_right_set(uint8_t speed, uint8_t dir);

/* Motor — compatibility wrappers */
void Motor_SetSpeed(int left, int right);   // → motor_set_differential()
void Motor_Stop(void);                      // → motor_stop()

/* Encoder */
void    Encoder_Init(void);
int32_t Encoder_GetLeft(void);
int32_t Encoder_GetRight(void);
void    Encoder_Reset(void);

/* UART */
void    UART_Init(void);
void    UART_SendChar(char c);
void    UART_SendString(const char *s);
uint8_t UART_Available(void);
char    UART_ReadChar(void);
```

---

## Development Rules
- **Branch:** all work on `main` — pull before you start, push after every 2-3 commits
- **NEVER** use Arduino APIs (`digitalWrite`, `analogRead`, `delay`, etc.)
- **NEVER** use blocking delays in the control loop
- **ALWAYS** read `firmware/config/pins.h` before writing a driver
- **ALWAYS** test your module independently before touching FSM
- **Disconnect HC-05 TX/RX wires before uploading code**

---

## Build Instructions
1. Install VS Code + PlatformIO extension
2. `git clone https://github.com/1Ferwiz/wall-following-robot.git`
3. Open folder in VS Code
4. Fix IntelliSense path (run once in PowerShell inside repo folder):
```powershell
(Get-Content .vscode\c_cpp_properties.json) -replace 'Alfred', $env:USERNAME | Set-Content .vscode\c_cpp_properties.json
```
5. Build: `pio run`
6. Upload: `pio run --target upload`

---

## Progress
- [x] Project structure
- [x] Pin assignment locked (`firmware/config/pins.h`)
- [x] PlatformIO configured (`platformio.ini`)
- [x] ADC driver
- [x] IR sensor driver
- [x] Ultrasonic driver
- [x] Motor driver (`motor.c`, `motor.h`)
- [ ] Encoder driver
- [ ] UART driver
- [ ] FSM implementation
