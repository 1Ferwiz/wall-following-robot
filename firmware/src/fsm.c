#include "fsm.h"
#include "encoder.h"
#include "ir_sensor.h"
#include "motor.h"
#include "uart.h"
#include "ultrasonic.h"
#include <util/atomic.h>
#include <stdbool.h>
#include <stdint.h>

extern volatile uint32_t millis;

typedef enum {
    IDLE = 0,
    FOLLOW_WALL,
    TURN_LEFT,
    TURN_RIGHT,
    AVOID,
    SEARCH,
    FINISHED
} State_t;

typedef enum {
    TURN_PHASE_BRAKE = 0,
    TURN_PHASE_ROTATE,
    TURN_PHASE_SETTLE
} TurnPhase_t;

typedef enum {
    AVOID_PHASE_BRAKE = 0,
    AVOID_PHASE_REVERSE,
    AVOID_PHASE_REALIGN
} AvoidPhase_t;

#define CONTROL_PERIOD_MS            20u
#define FRONT_WALL_MM               220u
#define FRONT_OPEN_MM               450u
#define COLLISION_MM                 70u
#define SIDE_WALL_PRESENT_MM        250u
#define SIDE_WALL_OPEN_MM           350u
#define RIGHT_TARGET_MM             120
#define BASE_PWM                    150
#define TURN_PWM                    145u
#define SEARCH_PWM                  115u
#define AVOID_PWM                   130u
#define TURN_DEBOUNCE_CYCLES          3u
#define FINISH_DEBOUNCE_CYCLES        3u
#define TURN_BRAKE_MS                50u
#define TURN_SETTLE_MS               60u
#define TURN_TIMEOUT_MS            1500u
#define AVOID_BRAKE_MS               40u
#define AVOID_REVERSE_CM              8u
#define AVOID_TIMEOUT_MS           1200u
#define SEARCH_TIMEOUT_MS          2500u
#define FINISHED_HOLD_MS            400u
#define MAX_TURNS                    32u
#define TX_PACKET_SIZE              128u

static State_t currentState = IDLE;
static State_t previousState = IDLE;
static uint32_t stateEntryMs = 0;
static uint32_t phaseEntryMs = 0;
static TurnPhase_t turnPhase = TURN_PHASE_BRAKE;
static AvoidPhase_t avoidPhase = AVOID_PHASE_BRAKE;

static char turnSequence[MAX_TURNS + 1u];
static uint8_t turnCount = 0;
static bool loggedTurn = false;
static uint8_t leftCandidateCount = 0;
static uint8_t rightCandidateCount = 0;
static uint8_t finishCandidateCount = 0;
static uint8_t followCycles = 0;
static uint16_t leftFiltered = 0;
static uint16_t rightFiltered = 0;
static int16_t pidIntegral = 0;
static int16_t pidPrevError = 0;

static char txPacket[TX_PACKET_SIZE];
static uint8_t txIndex = 0;
static uint8_t txLength = 0;
static bool txDone = false;

static uint8_t startMatch = 0;

static uint32_t now_ms(void)
{
    uint32_t now;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        now = millis;
    }

    return now;
}

static int16_t clamp_i16(int16_t value, int16_t minValue, int16_t maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static uint16_t abs_i32(int32_t value)
{
    return (value < 0) ? (uint16_t)(-value) : (uint16_t)value;
}

static uint16_t average_ticks(void)
{
    uint16_t left = abs_i32(Encoder_GetLeft());
    uint16_t right = abs_i32(Encoder_GetRight());
    return (uint16_t)(((uint32_t)left + (uint32_t)right) / 2u);
}

static bool side_present(uint16_t distance)
{
    return (distance > 0u) && (distance <= SIDE_WALL_PRESENT_MM);
}

static bool side_open(uint16_t distance)
{
    return (distance == 0u) || (distance >= SIDE_WALL_OPEN_MM);
}

static bool front_wall(uint16_t distance)
{
    return (distance > 0u) && (distance <= FRONT_WALL_MM);
}

static bool front_open(uint16_t distance)
{
    return (distance == 0u) || (distance >= FRONT_OPEN_MM);
}

static void enter_state(State_t next)
{
    previousState = currentState;
    currentState = next;
    stateEntryMs = now_ms();

    if (next == TURN_LEFT || next == TURN_RIGHT)
    {
        turnPhase = TURN_PHASE_BRAKE;
        phaseEntryMs = stateEntryMs;
        Encoder_Reset();
        motor_brake();
    }
    else if (next == AVOID)
    {
        avoidPhase = AVOID_PHASE_BRAKE;
        phaseEntryMs = stateEntryMs;
        Encoder_Reset();
        motor_brake();
    }
    else if (next == SEARCH)
    {
        Encoder_Reset();
        motor_turn_right(SEARCH_PWM);
    }
    else if (next == FOLLOW_WALL)
    {
        followCycles = 0;
    }
    else if (next == FINISHED)
    {
        motor_brake();
        txIndex = 0;
        txLength = 0;
        txDone = false;
    }
}

static void reset_run(void)
{
    turnCount = 0;
    turnSequence[0] = '\0';
    loggedTurn = false;
    leftCandidateCount = 0;
    rightCandidateCount = 0;
    finishCandidateCount = 0;
    followCycles = 0;
    leftFiltered = 0;
    rightFiltered = 0;
    pidIntegral = 0;
    pidPrevError = 0;
    txIndex = 0;
    txLength = 0;
    txDone = false;
    startMatch = 0;
    Encoder_Reset();
}

static void log_turn(char turn)
{
    if (turnCount < MAX_TURNS)
    {
        turnSequence[turnCount] = turn;
        turnCount++;
        turnSequence[turnCount] = '\0';
        loggedTurn = true;
    }
}

static uint16_t filter_distance(uint16_t previous, uint16_t sample)
{
    if (previous == 0u)
    {
        return sample;
    }

    if (sample == 0u)
    {
        sample = 800u;
    }

    return (uint16_t)(((uint32_t)previous * 3u + sample) / 4u);
}

static void read_side_sensors(uint16_t *left, uint16_t *right)
{
    leftFiltered = filter_distance(leftFiltered, IR_GetLeftDistance());
    rightFiltered = filter_distance(rightFiltered, IR_GetRightDistance());
    *left = leftFiltered;
    *right = rightFiltered;
}

static bool start_received(void)
{
    static const char startWord[] = "START";

    while (UART_Available())
    {
        char c = UART_ReadChar();

        if (c == 's' || c == 'S')
        {
            startMatch = 1u;
            return true;
        }

        if (c >= 'a' && c <= 'z')
        {
            c = (char)(c - ('a' - 'A'));
        }

        if (c == startWord[startMatch])
        {
            startMatch++;
            if (startMatch == 5u)
            {
                startMatch = 0u;
                return true;
            }
        }
        else
        {
            startMatch = (c == 'S') ? 1u : 0u;
        }
    }

    return false;
}

static void wall_follow(uint16_t rightDistance)
{
    int16_t correction = 0;

    if (rightDistance > 0u)
    {
        int16_t error = (int16_t)rightDistance - RIGHT_TARGET_MM;
        int16_t derivative = error - pidPrevError;

        pidIntegral = clamp_i16((int16_t)(pidIntegral + error), -500, 500);
        correction = (int16_t)((error / 2) + (pidIntegral / 80) + derivative);
        correction = clamp_i16(correction, -70, 70);
        pidPrevError = error;
    }

    motor_set_differential((int16_t)(BASE_PWM + correction),
                           (int16_t)(BASE_PWM - correction));
}

static void append_char(char c)
{
    if (txLength < (TX_PACKET_SIZE - 1u))
    {
        txPacket[txLength++] = c;
        txPacket[txLength] = '\0';
    }
}

static void append_string(const char *text)
{
    while (*text != '\0')
    {
        append_char(*text);
        text++;
    }
}

static void append_u8(uint8_t value)
{
    char digits[3];
    uint8_t i = 0;

    if (value == 0u)
    {
        append_char('0');
        return;
    }

    while (value > 0u && i < sizeof(digits))
    {
        digits[i++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (i > 0u)
    {
        append_char(digits[--i]);
    }
}

static void build_report(void)
{
    txLength = 0;
    txPacket[0] = '\0';

    append_string("Turns: ");
    append_u8(turnCount);
    append_string("\r\nSequence: ");

    for (uint8_t i = 0; i < turnCount; i++)
    {
        append_char(turnSequence[i]);
        if ((uint8_t)(i + 1u) < turnCount)
        {
            append_string(", ");
        }
    }

    append_string("\r\n");
}

static void send_report_step(void)
{
    if (txLength == 0u)
    {
        build_report();
    }

    while (txIndex < txLength)
    {
        if (!UART_WriteByte((uint8_t)txPacket[txIndex]))
        {
            return;
        }
        txIndex++;
    }

    txDone = true;
}

static void update_follow_wall(void)
{
    uint16_t left;
    uint16_t right;
    uint16_t front;
    bool leftOpen;
    bool rightOpen;
    bool leftPresent;
    bool rightPresent;
    bool frontIsWall;
    bool openArea;

    Ultrasonic_Trigger();
    read_side_sensors(&left, &right);
    front = Ultrasonic_GetDistance();

    leftOpen = side_open(left);
    rightOpen = side_open(right);
    leftPresent = side_present(left);
    rightPresent = side_present(right);
    frontIsWall = front_wall(front);
    openArea = front_open(front) && leftOpen && rightOpen;

    if ((front > 0u) && (front <= COLLISION_MM))
    {
        enter_state(AVOID);
        return;
    }

    if (frontIsWall && rightPresent && leftOpen)
    {
        if (leftCandidateCount < TURN_DEBOUNCE_CYCLES) leftCandidateCount++;
    }
    else
    {
        leftCandidateCount = 0;
    }

    if (frontIsWall && leftPresent && rightOpen)
    {
        if (rightCandidateCount < TURN_DEBOUNCE_CYCLES) rightCandidateCount++;
    }
    else
    {
        rightCandidateCount = 0;
    }

    if (leftCandidateCount >= TURN_DEBOUNCE_CYCLES)
    {
        log_turn('L');
        enter_state(TURN_LEFT);
        return;
    }

    if (rightCandidateCount >= TURN_DEBOUNCE_CYCLES)
    {
        log_turn('R');
        enter_state(TURN_RIGHT);
        return;
    }

    if (openArea && loggedTurn && followCycles >= 3u)
    {
        if (finishCandidateCount < FINISH_DEBOUNCE_CYCLES) finishCandidateCount++;
    }
    else
    {
        finishCandidateCount = 0;
    }

    if (finishCandidateCount >= FINISH_DEBOUNCE_CYCLES)
    {
        enter_state(FINISHED);
        return;
    }

    wall_follow(right);

    if (followCycles < 255u)
    {
        followCycles++;
    }
}

static void update_turn(State_t turnState)
{
    uint32_t elapsed = now_ms() - phaseEntryMs;

    if (turnPhase == TURN_PHASE_BRAKE)
    {
        if (elapsed >= TURN_BRAKE_MS)
        {
            Encoder_Reset();
            if (turnState == TURN_LEFT)
            {
                motor_turn_left(TURN_PWM);
            }
            else
            {
                motor_turn_right(TURN_PWM);
            }
            turnPhase = TURN_PHASE_ROTATE;
            phaseEntryMs = now_ms();
        }
        return;
    }

    if (turnPhase == TURN_PHASE_ROTATE)
    {
        elapsed = now_ms() - phaseEntryMs;
        if (Encoder_TurnComplete(TICKS_FOR_90_TURN) || elapsed >= TURN_TIMEOUT_MS)
        {
            motor_brake();
            turnPhase = TURN_PHASE_SETTLE;
            phaseEntryMs = now_ms();
        }
        return;
    }

    if ((now_ms() - phaseEntryMs) >= TURN_SETTLE_MS)
    {
        uint16_t left;
        uint16_t right;

        read_side_sensors(&left, &right);
        if (side_present(left) || side_present(right))
        {
            enter_state(FOLLOW_WALL);
        }
        else
        {
            enter_state(SEARCH);
        }
    }
}

static void update_avoid(void)
{
    uint32_t elapsed = now_ms() - phaseEntryMs;

    if (avoidPhase == AVOID_PHASE_BRAKE)
    {
        if (elapsed >= AVOID_BRAKE_MS)
        {
            Encoder_Reset();
            motor_backward(AVOID_PWM);
            avoidPhase = AVOID_PHASE_REVERSE;
            phaseEntryMs = now_ms();
        }
        return;
    }

    if (avoidPhase == AVOID_PHASE_REVERSE)
    {
        if (average_ticks() >= Encoder_CmToTicks((float)AVOID_REVERSE_CM) ||
            elapsed >= AVOID_TIMEOUT_MS)
        {
            Encoder_Reset();
            motor_turn_right(SEARCH_PWM);
            avoidPhase = AVOID_PHASE_REALIGN;
            phaseEntryMs = now_ms();
        }
        return;
    }

    if (avoidPhase == AVOID_PHASE_REALIGN)
    {
        uint16_t left;
        uint16_t right;

        read_side_sensors(&left, &right);
        if (side_present(right) || side_present(left) ||
            (now_ms() - phaseEntryMs) >= AVOID_TIMEOUT_MS)
        {
            enter_state(FOLLOW_WALL);
        }
    }
}

static void update_search(void)
{
    uint16_t left;
    uint16_t right;

    read_side_sensors(&left, &right);

    if (side_present(left) || side_present(right))
    {
        enter_state(FOLLOW_WALL);
        return;
    }

    if ((now_ms() - stateEntryMs) >= SEARCH_TIMEOUT_MS)
    {
        motor_brake();
        enter_state(IDLE);
    }
}

void FSM_Init(void)
{
    IR_Init();
    Ultrasonic_Init();
    motor_init();
    motor_enable();
    Encoder_Init();
    UART_Init();

    reset_run();
    currentState = IDLE;
    previousState = IDLE;
    stateEntryMs = now_ms();
    phaseEntryMs = stateEntryMs;
    motor_stop();
}

void FSM_Update(void)
{
    switch (currentState)
    {
        case IDLE:
            motor_stop();
            if (start_received())
            {
                reset_run();
                enter_state(FOLLOW_WALL);
            }
            break;

        case FOLLOW_WALL:
            update_follow_wall();
            break;

        case TURN_LEFT:
        case TURN_RIGHT:
            update_turn(currentState);
            break;

        case AVOID:
            update_avoid();
            break;

        case SEARCH:
            update_search();
            break;

        case FINISHED:
            send_report_step();
            if (txDone && (now_ms() - stateEntryMs) >= FINISHED_HOLD_MS)
            {
                reset_run();
                enter_state(IDLE);
            }
            break;

        default:
            motor_brake();
            enter_state(IDLE);
            break;
    }
}

uint8_t FSM_IsRunning(void)
{
    return (currentState != IDLE && currentState != FINISHED) ? 1u : 0u;
}

uint8_t FSM_GetTurnCount(void)
{
    return turnCount;
}

const char *FSM_GetTurnSequence(void)
{
    return turnSequence;
}
