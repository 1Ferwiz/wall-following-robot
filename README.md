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
| Motor Driver | 2x L298N dual H-bridge modules, one module per motor |
| Bluetooth | HC-05 breakout over USART0 |
| Start Switch | 2-pin ON/OFF switch |

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
| Left L298N ENA | D5 | PD5 / OC0B | Left motor PWM enable |
| Left L298N IN1 | A2 | PC2 | Left motor direction input 1 |
| Left L298N IN2 | A3 | PC3 | Left motor direction input 2 |
| Right L298N ENA | D6 | PD6 / OC0A | Right motor PWM enable |
| Right L298N IN1 | A4 | PC4 | Right motor direction input 1 |
| Right L298N IN2 | A5 | PC5 | Right motor direction input 2 |
| Encoder L A | D2 | PD2 / INT0 | Left motor tick input |
| Encoder R A | D3 | PD3 / INT1 | Right motor tick input |
| Encoder L B | D9 | PB1 | Left encoder B channel |
| Encoder R B | D10 | PB2 | Right encoder B channel |
| Start switch | D12 | PB4 | Input with internal pull-up, ON connects to GND |
| UART TX | D1 | PD1 | To HC-05 RX |
| UART RX | D0 | PD0 | From HC-05 TX |

## Exact Wiring
All grounds must be connected together: Arduino GND, both L298N GND pins, sensor GND,
HC-05 GND, encoder GND, and battery negative.

### Power
| From | To | Notes |
|---|---|---|
| 12V battery positive | Left L298N +12V / VS | Left motor driver power |
| 12V battery positive | Right L298N +12V / VS | Right motor driver power |
| 12V battery negative | Common GND rail | Same ground as Arduino |
| Arduino GND | Left L298N GND | Common logic and motor reference |
| Arduino GND | Right L298N GND | Common logic and motor reference |
| Arduino 5V | Left L298N 5V logic pin | Only if that L298N board's 5V regulator jumper is removed |
| Arduino 5V | Right L298N 5V logic pin | Only if that L298N board's 5V regulator jumper is removed |
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
Use one L298N board for the left motor and one L298N board for the right motor.
Only channel A is used on each board. The unused channel B pins on both boards
can stay unwired.

Remove the ENA jumper from each L298N board because Arduino D5/D6 provide PWM
speed control. Do not confuse the ENA jumper with the board's 5V regulator
jumper.

| From | To | Controls |
|---|---|---|
| Arduino D5 / PD5 / OC0B | Left L298N ENA | Left motor PWM/speed |
| Arduino A2 / PC2 | Left L298N IN1 | Left motor forward direction input |
| Arduino A3 / PC3 | Left L298N IN2 | Left motor reverse direction input |
| Arduino D6 / PD6 / OC0A | Right L298N ENA | Right motor PWM/speed |
| Arduino A4 / PC4 | Right L298N IN1 | Right motor forward direction input |
| Arduino A5 / PC5 | Right L298N IN2 | Right motor reverse direction input |
| 12V battery positive | Left L298N +12V / VS | Left motor driver power |
| 12V battery positive | Right L298N +12V / VS | Right motor driver power |
| Common GND rail | Left L298N GND | Logic and motor reference |
| Common GND rail | Right L298N GND | Logic and motor reference |

### L298N Jumpers
| Jumper | Left L298N | Right L298N | Notes |
|---|---|---|---|
| ENA jumper | Removed | Removed | Arduino PWM must connect to ENA |
| ENB jumper | Doesn't matter | Doesn't matter | Channel B is unused |
| 5V regulator jumper | Installed when feeding +12V | Installed when feeding +12V | If removed, feed Arduino 5V into that board's 5V pin |

The motor-output screw terminals are:

| L298N Output | Connect To |
|---|---|
| Left L298N OUT1 | Left motor `M+` |
| Left L298N OUT2 | Left motor `M-` |
| Right L298N OUT1 | Right motor `M+` |
| Right L298N OUT2 | Right motor `M-` |

### Start Switch
The start switch is a 2-pin ON/OFF switch. The firmware enables Arduino's
internal pull-up on D12, so no external resistor is needed.

| From | To | Notes |
|---|---|---|
| Switch pin 1 | Arduino D12 / PB4 | Start input |
| Switch pin 2 | Arduino GND | ON closes D12 to GND |

When the switch is OFF, the robot stays stopped and the run state is reset.
When the switch is ON, the robot starts wall following.

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
| `M+` motor power | Red | Left L298N OUT1 |
| `M-` motor power | Black | Left L298N OUT2 |
| Encoder `VCC` | Yellow | Arduino 5V |
| Encoder `GND` | White | Arduino GND |
| Encoder `A` | Green | Arduino D2 / PD2 / INT0 |
| Encoder `B` | Blue | Arduino D9 / PB1 |

### Right GA25-370 Motor
| Motor Wire / PCB Label | Common Color | Connect To |
|---|---|---|
| `M+` motor power | Red | Right L298N OUT1 |
| `M-` motor power | Black | Right L298N OUT2 |
| Encoder `VCC` | Yellow | Arduino 5V |
| Encoder `GND` | White | Arduino GND |
| Encoder `A` | Green | Arduino D3 / PD3 / INT1 |
| Encoder `B` | Blue | Arduino D10 / PB2 |

If a motor spins backward during the forward test, swap only that motor's
`M+` and `M-` wires on its L298N motor output. Do not swap encoder `VCC`
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
- The FSM starts from `FSM_IDLE` and keeps motors stopped while the D12 start
  switch is OFF.
- When the D12 start switch is ON, the FSM waits `FSM_START_DELAY_MS`, then
  enters wall following.
- Turning the D12 start switch OFF stops the motors and resets the run state.
- Front ultrasonic detects turn entry.
- Left/right ultrasonic readings classify whether the open turn is left or right.
- Turn states currently use `MOTOR_TURN_DURATION_MS` only. Encoder-based turn
  stopping is disabled until the car is mechanically reliable.
- Finish detection stops the car after at least one completed turn when front,
  left, and right all stay open for `FSM_FINISH_OPEN_MS`. It is ignored for
  `FSM_FINISH_ARM_MS` after each turn to avoid false stops at corners.
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
- The start switch uses D12 with the internal pull-up; ON connects D12 to GND.
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
C:\Users\Omart\.platformio\penv\Scripts\platformio.exe run
```
