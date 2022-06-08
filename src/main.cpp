#include <Arduino.h>

// Enable this if you need to setup this project with a new remote
// #define FIND_DECODER_AND_TRIGGER_VALUE

// Enable this to find the desired servo degree settings
// #define FIND_SERVO_POSITIONS
#define SERVO_FIND_POS_INC 10

// Enable this to find the desired servo pulse width for the widest possible movement range
// #define FIND_SERVO_PULSE_WIDTHS
#define SERVO_FIND_PULSE_WIDTH_INC 50

// I only need this decoder!
#ifndef FIND_DECODER_AND_TRIGGER_VALUE
#define DECODE_NEC
#define NO_LED_FEEDBACK_CODE
#endif
#include <IRremote.hpp> // uses timer 2

// PIN configuration:
// TODO improve layout to keep all pins on one side (combined with power & gnd pins)
#define IR_RECEIVER_PIN 2
#define SERVO_PIN 3

// IR Remote configuration:
// The expected decodedRawData to use as trigger!
#define IR_TURN_ON_TRIGGER_VALUE 0xF30CFF1F
#define IR_TURN_OFF_TRIGGER_VALUE 0xEF10FF1F

// Servo configuration:
#define SERVO_MIN_PULSE_WIDTH 1000
#define SERVO_MAX_PULSE_WIDTH 2000
#define SERVO_NEUTRAL_POS 90
#define SERVO_SWITCH_ON_POS 0
#define SERVO_SWITCH_OFF_POS 180

#define CASE_STRINGIFY(x)   \
    case x:                 \
        Serial.println(#x); \
        break

#ifndef FIND_DECODER_AND_TRIGGER_VALUE
#include <Servo.h> // uses Timer 1
static Servo servo;

static void moveToPos(uint8_t pos) {
    if (servo.attached()) {
        servo.write(pos, 500 /*hold this position for 500ms*/);
        delay(500);
    }
}

static void returnToNeutral() {
    moveToPos(SERVO_NEUTRAL_POS);
}

static void triggerSwitch(uint8_t pos) {
    moveToPos(pos);
    delay(500);
    returnToNeutral();
}

static void initServo(uint16_t minPulseWidth = SERVO_MIN_PULSE_WIDTH, uint16_t maxPulseWidth = SERVO_MAX_PULSE_WIDTH) {
    if (servo.attached()) {
        servo.detach();
    }
    servo.attach(SERVO_PIN, minPulseWidth, maxPulseWidth);
}
#endif

void setup() {
#if defined(FIND_DECODER_AND_TRIGGER_VALUE) || defined(FIND_SERVO_POSITIONS) || defined(FIND_SERVO_PULSE_WIDTHS)
    Serial.begin(9600);
#endif

    IrReceiver.begin(IR_RECEIVER_PIN,
#ifndef NO_LED_FEEDBACK_CODE
                     // enable blinking of the built-in LED when an IR signal is received
                     true /*aEnableLEDFeedback*/,
#else
                     false /*aEnableLEDFeedback*/,
#endif
                     0 /*aFeedbackLEDPin*/);

#ifdef FIND_DECODER_AND_TRIGGER_VALUE
    uint32_t key_value = 0;
    for (;;) {
        if (IRData *data = IrReceiver.read()) {
            auto rawData = data->decodedRawData;
            if (rawData == 0XFFFFFFFF) {
                rawData = key_value;
            }
            key_value = rawData;
            Serial.print("0x");
            Serial.println(rawData, HEX);
            switch (data->protocol) {
                CASE_STRINGIFY(PULSE_DISTANCE);
                CASE_STRINGIFY(PULSE_WIDTH);
                CASE_STRINGIFY(DENON);
                CASE_STRINGIFY(DISH);
                CASE_STRINGIFY(JVC);
                CASE_STRINGIFY(LG);
                CASE_STRINGIFY(LG2);
                CASE_STRINGIFY(NEC);
                CASE_STRINGIFY(PANASONIC);
                CASE_STRINGIFY(KASEIKYO);
                CASE_STRINGIFY(KASEIKYO_JVC);
                CASE_STRINGIFY(KASEIKYO_DENON);
                CASE_STRINGIFY(KASEIKYO_SHARP);
                CASE_STRINGIFY(KASEIKYO_MITSUBISHI);
                CASE_STRINGIFY(RC5);
                CASE_STRINGIFY(RC6);
                CASE_STRINGIFY(SAMSUNG);
                CASE_STRINGIFY(SHARP);
                CASE_STRINGIFY(SONY);
                CASE_STRINGIFY(ONKYO);
                CASE_STRINGIFY(APPLE);
                CASE_STRINGIFY(BOSEWAVE);
                CASE_STRINGIFY(LEGO_PF);
                CASE_STRINGIFY(MAGIQUEST);
                CASE_STRINGIFY(WHYNTER);
                default:
                    CASE_STRINGIFY(UNKNOWN);
            }
            IrReceiver.resume();
        }
    }
#else
    initServo();
    returnToNeutral();

#ifdef FIND_SERVO_POSITIONS
    uint8_t servoPos = SERVO_NEUTRAL_POS;
    for (;;) {
        if (IRData *data = IrReceiver.read()) {
            // TODO use command and address instead?
            auto rawData = data->decodedRawData;
            uint8_t oldPos = servoPos;
            if (rawData == IR_TURN_ON_TRIGGER_VALUE) {
                servoPos += SERVO_FIND_POS_INC;
            } else if (rawData == IR_TURN_OFF_TRIGGER_VALUE) {
                servoPos -= SERVO_FIND_POS_INC;
            }

            if (oldPos != servoPos) {
                Serial.print("Current servo position: ");
                Serial.println(servoPos);
                moveToPos(servoPos);
            }
            IrReceiver.resume();
        }
    }

#elif defined(FIND_SERVO_PULSE_WIDTHS)
    uint16_t minPulseWidth = SERVO_MIN_PULSE_WIDTH;
    uint16_t maxPulseWidth = SERVO_MAX_PULSE_WIDTH;
    for (;;) {
        if (IRData *data = IrReceiver.read()) {
            // TODO use command and address instead?
            auto rawData = data->decodedRawData;
            uint8_t targetPos = SERVO_NEUTRAL_POS;
            if (rawData == IR_TURN_ON_TRIGGER_VALUE) {
                maxPulseWidth += SERVO_FIND_PULSE_WIDTH_INC;
                targetPos = SERVO_SWITCH_ON_POS;
            } else if (rawData == IR_TURN_OFF_TRIGGER_VALUE) {
                minPulseWidth -= SERVO_FIND_PULSE_WIDTH_INC;
                targetPos = SERVO_SWITCH_OFF_POS;
            }

            if (targetPos != SERVO_NEUTRAL_POS) {
                Serial.print("New values: min: ");
                Serial.print(minPulseWidth);
                Serial.print(" max: ");
                Serial.println(maxPulseWidth);
                initServo(minPulseWidth, maxPulseWidth);
                moveToPos(targetPos);
            }
            IrReceiver.resume();
        }
    }
#else
    for (;;) {
        if (IRData *data = IrReceiver.read()) {
            // TODO use command and address instead?
            auto rawData = data->decodedRawData;
            if (rawData == IR_TURN_ON_TRIGGER_VALUE) {
                triggerSwitch(SERVO_SWITCH_ON_POS);
            } else if (rawData == IR_TURN_OFF_TRIGGER_VALUE) {
                triggerSwitch(SERVO_SWITCH_OFF_POS);
            }
            // Else ignore the data
            IrReceiver.resume();
        }
    }
#endif
#endif
}

void loop() {
    // TODO we might want to move our main loops here
}
