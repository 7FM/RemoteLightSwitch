#include <Arduino.h>

// TODO enable only the needed decoders!
// #define NO_LED_FEEDBACK_CODE
#include <IRremote.hpp>

#define IR_RECEIVER_PIN 2 // define the IR receiver pin

#define CASE_STRINGIFY(x)   \
    case x:                 \
        Serial.println(#x); \
        break

void setup() {
    Serial.begin(9600);

    IrReceiver.begin(IR_RECEIVER_PIN,
#ifndef NO_LED_FEEDBACK_CODE
                     // enable blinking of the built-in LED when an IR signal is received
                     true /*aEnableLEDFeedback*/,
#else
                     false /*aEnableLEDFeedback*/,
#endif
                     0 /*aFeedbackLEDPin*/);

    uint32_t key_value = 0;
    // IRData *data = nullptr;
    for (;;) {
        if (IRData *data = IrReceiver.read()) {
            auto rawData = data->decodedRawData;
            if (rawData == 0XFFFFFFFF) {
                rawData = key_value;
            }
            key_value = rawData;
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
}

void loop() {
    // put your main code here, to run repeatedly:
}
