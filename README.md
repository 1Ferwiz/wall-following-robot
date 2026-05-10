# Wall Following Robot - Team 11

## Overview
Autonomous wall-following robot built on AVR ATmega328P / Arduino Uno R3.
The robot follows corridor walls, detects 90-degree turns, counts/classifies
turns as `L` or `R`, and sends the final report to a PC through HC-05 Bluetooth.

## Current Hardware Configuration
| Component | Details |
|---|---|
| MCU | Arduino Uno R3 - AVR ATmega328P @ 16 MHz |
| Wall Sensors | 3x HC-SR04 ultrasonic sensors |
| Motors | 2x GA25-370 12V 250RPM encoder motors |
| Motor Driver | L298N dual H-bridge motor driver |
| Bluetooth | HC-05 breakout over USART0 |

## Sensor Roles
| Sensor | Role |
|---|---|
| Left HC-SR04 | Measures distance to left wall for wall following |
| Right HC-SR04 | Measures distance to right wall for wall following |
| Front HC-SR04 | Detects front wall / turn entry |

## Arduino Pin Assignment
Read `firmware/config/pins.h` before wiring or changing any driver.
This table lists Arduino pins used by the firmware. The motor rows are
Arduino-to-L298N control pins, not the six physical GA25 motor wires.

| Signal | Arduino Pin | AVR Pin | Notes |
|---|---|---|---|
| Front US TRIG | D4 | PD4 | Digital output |
| Front US ECHO | D8 | PB0 | Timed using Timer1 counter |
| Left US TRIG | A0 | PC0 | Digital output |
| Left US ECHO | A1 | PC1 | Digital input |
| Right US TRIG | D7 | PD7 | Digital output |
| Right US ECHO | D11 | PB3 | Digital input |
| L298N ENA | D5 | PD5 / OC0B | Left motor PWM enable |
| L298N IN1 | A2 | PC2 | Left motor direction input 1 |
| L298N IN2 | A3 | PC3 | Left motor direction input 2 |
| L298N ENB | D6 | PD6 / OC0A | Right motor PWM enable |
| L298N IN3 | A4 | PC4 | Right motor direction input 1 |
| L298N IN4 | A5 | PC5 | Right motor direction input 2 |
| Encoder L A | D2 | PD2 / INT0 | Left motor tick input |
| Encoder R A | D3 | PD3 / INT1 | Right motor tick input |
| Encoder L B | D9 | PB1 | Left encoder B channel |
| Encoder R B | D10 | PB2 | Right encoder B channel |
| UART TX | D1 | PD1 | To HC-05 RX |
| UART RX | D0 | PD0 | From HC-05 TX |

## Exact Wiring
All grounds must be connected together: Arduino GND, L298N GND, sensor GND,
HC-05 GND, encoder GND, and battery negative.

### Power
| From | To | Notes |
|---|---|---|
| 12V battery positive | L298N +12V / VS | Motor driver power |
| 12V battery negative | Common GND rail | Same ground as Arduino |
| Arduino GND | L298N GND | Common logic and motor reference |
| Arduino 5V | L298N 5V logic pin | Only if the L298N board's 5V regulator jumper is removed |
| Arduino 5V | HC-SR04 VCC pins | Front, left, and right sensors |
| Arduino GND | HC-SR04 GND pins | Front, left, and right sensors |
| Arduino 5V | GA25 encoder VCC wires | Both motor encoder boards |
| Arduino GND | GA25 encoder GND wires | Both motor encoder boards |
| Arduino 5V | HC-05 VCC | For common HC-05 breakout boards with onboard regulator |
| Arduino GND | HC-05 GND | Common ground |

### Front HC-SR04
| From | To |
|---|---|
| Arduino 5V | Front HC-SR04 VCC |
| Arduino GND | Front HC-SR04 GND |
| Arduino D4 / PD4 | Front HC-SR04 TRIG |
| Arduino D8 / PB0 | Front HC-SR04 ECHO |

### Left HC-SR04
| From | To |
|---|---|
| Arduino 5V | Left HC-SR04 VCC |
| Arduino GND | Left HC-SR04 GND |
| Arduino A0 / PC0 | Left HC-SR04 TRIG |
| Arduino A1 / PC1 | Left HC-SR04 ECHO |

### Right HC-SR04
| From | To |
|---|---|
| Arduino 5V | Right HC-SR04 VCC |
| Arduino GND | Right HC-SR04 GND |
| Arduino D7 / PD7 | Right HC-SR04 TRIG |
| Arduino D11 / PB3 | Right HC-SR04 ECHO |

### Arduino to L298N Control Wires
These are the 6 control wires from Arduino to the motor driver. They are not
the motor's 6-wire cable. Remove the ENA/ENB jumpers if your L298N board has
them installed, otherwise Arduino PWM on D5/D6 will be bypassed.
Do not confuse ENA/ENB jumpers with the board's 5V regulator jumper.

| From | To | Controls |
|---|---|---|
| Arduino D5 / PD5 / OC0B | L298N ENA | Left motor PWM/speed |
| Arduino A2 / PC2 | L298N IN1 | Left motor forward direction input |
| Arduino A3 / PC3 | L298N IN2 | Left motor reverse direction input |
| Arduino D6 / PD6 / OC0A | L298N ENB | Right motor PWM/speed |
| Arduino A4 / PC4 | L298N IN3 | Right motor forward direction input |
| Arduino A5 / PC5 | L298N IN4 | Right motor reverse direction input |
| 12V battery positive | L298N +12V / VS | Motor driver power |
| Common GND rail | L298N GND | Logic and motor reference |

The motor-output screw terminals are:

| L298N Output | Connect To |
|---|---|
| OUT1 | Left motor `M+` |
| OUT2 | Left motor `M-` |
| OUT3 | Right motor `M+` |
| OUT4 | Right motor `M-` |

### GA25-370 Motor Wiring - 6 Wires Per Motor
Each GA25 motor has 6 wires total:
- 2 motor power wires go to the L298N motor output.
- 4 encoder wires go to Arduino 5V, GND, and encoder signal pins.

So each motor has exactly 6 physical motor wires, while the L298N also has
separate Arduino control wires listed in the previous table.

Wire colors can vary by seller. If the motor PCB has labels, trust the labels
more than color. Common labels are `M+`, `M-`, `VCC`, `GND`, `A`, and `B`.

### Left GA25-370 Motor
| Motor Wire / PCB Label | Common Color | Connect To |
|---|---|---|
| `M+` motor power | Red | L298N OUT1 |
| `M-` motor power | Black | L298N OUT2 |
| Encoder `VCC` | Yellow | Arduino 5V |
| Encoder `GND` | White | Arduino GND |
| Encoder `A` | Green | Arduino D2 / PD2 / INT0 |
| Encoder `B` | Blue | Arduino D9 / PB1 |

### Right GA25-370 Motor
| Motor Wire / PCB Label | Common Color | Connect To |
|---|---|---|
| `M+` motor power | Red | L298N OUT3 |
| `M-` motor power | Black | L298N OUT4 |
| Encoder `VCC` | Yellow | Arduino 5V |
| Encoder `GND` | White | Arduino GND |
| Encoder `A` | Green | Arduino D3 / PD3 / INT1 |
| Encoder `B` | Blue | Arduino D10 / PB2 |

If a motor spins backward during the forward test, swap only that motor's
`M+` and `M-` wires on the L298N motor output. Do not swap encoder `VCC`
and `GND`.

### HC-05 Bluetooth
Disconnect HC-05 TX/RX from D0/D1 while uploading firmware.

| From | To | Notes |
|---|---|---|
| Arduino 5V | HC-05 VCC | Breakout board VCC |
| Arduino GND | HC-05 GND | Common ground |
| HC-05 TXD | Arduino D0 / PD0 / RXD | AVR receives Bluetooth data |
| Arduino D1 / PD1 / TXD | HC-05 RXD through level divider | AVR sends Bluetooth data |
| Arduino D1 / PD1 / TXD | 1 kOhm resistor, then HC-05 RXD | Divider top resistor |
| HC-05 RXD | 2 kOhm resistor, then GND | Divider bottom resistor |
| HC-05 STATE | Not connected | Optional |
| HC-05 EN/KEY | Not connected | Optional |

## Timer Assignment
| Timer | Role |
|---|---|
| Timer0 | Motor PWM, OC0B=D5 left and OC0A=D6 right |
| Timer1 | Free-running ultrasonic echo timing for all HC-SR04 sensors |
| Timer2 | 1 ms system tick through `SysTick` |

## Module Interfaces
```c
/* System tick */
void SysTick_Init(void);
uint32_t SysTick_GetMs(void);

/* Ultrasonic */
void Ultrasonic_Init(void);
void Ultrasonic_Update(void);
uint16_t Ultrasonic_GetDistance(US_Sensor_t sensor);

/* Motors */
void Motor_Init(void);
void Motor_SetSpeed(int left, int right);
void Motor_Stop(void);
void Motor_Forward(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_SlowDown(void);

/* Encoders */
void Encoder_Init(void);
int32_t Encoder_GetLeft(void);
int32_t Encoder_GetRight(void);
void Encoder_Reset(void);
float Encoder_TicksToCm(int32_t ticks);
uint32_t Encoder_CmToTicks(float cm);
uint8_t Encoder_TurnComplete(uint32_t target_ticks);

/* UART / HC-05 */
void UART_Init(void);
bool UART_WriteByte(uint8_t byte);
uint8_t UART_Write(const uint8_t *data, uint8_t length);
void UART_SendChar(char c);
void UART_SendString(const char *text);
void UART_SendTurnReport(uint8_t count, const char *sequence);
bool UART_RxOverflowed(void);
bool UART_TxOverflowed(void);

/* FSM */
void FSM_Init(void);
void FSM_Update(void);
FSM_State_t FSM_GetState(void);
uint8_t FSM_GetTurnCount(void);
const char *FSM_GetTurnSequence(void);
```

## Runtime Behavior
- `main.c` initializes the system tick, motors, ultrasonic sensors, encoders,
  UART, and FSM.
- `Ultrasonic_Update()` runs every 20 ms and measures one HC-SR04 at a time:
  front, left, right, then repeats.
- `FSM_Update()` runs every 1 ms.
- The FSM starts from `FSM_IDLE`, waits 500 ms, then enters wall following.
- Front ultrasonic detects turn entry.
- Left/right ultrasonic readings classify whether the open turn is left or right.
- Turn states stop using GA25 encoder ticks, with `MOTOR_TURN_DURATION_MS` as
  a timed fallback.
- If front, left, and right are all open after at least one logged turn, the FSM
  treats that stable open area as the finish condition.
- On complete, UART sends:
```text
Turns: N
Sequence: L, R, ...
```

## Development Rules
- Use register-level AVR C.
- Do not use Arduino APIs such as `digitalWrite`, `analogRead`, or `delay`.
- Do not use blocking delays in the main control loop.
- Keep HC-05 disconnected from D0/D1 while uploading code.
- GA25 encoder ticks are counted on D2/D3 through INT0/INT1.
- UART uses interrupt-driven RX/TX ring buffers at 9600 8N1.
- Ultrasonic trigger pulses use the AVR `_delay_us()` helper for the required
  10 us HC-SR04 pulse; no Arduino timing API is used.

## Tuning Values
Tune these on the real hardware:
- `MOTOR_BASE_SPEED`
- `MOTOR_TURN_SPEED`
- `MOTOR_TURN_DURATION_MS`
- `TICKS_FOR_90_TURN`
- `WALL_TARGET_MM`
- `FSM_FRONT_SLOW_MM`
- `FSM_FRONT_TURN_MM`
- `FSM_OPEN_SIDE_MM`
- Encoder constants in `firmware/inc/encoder.h`

## Build
```powershell
pio run
```

If `pio` is not on PATH, use:
```powershell
C:\Users\mods4\.platformio\penv\Scripts\platformio.exe run
```
