#include <Arduino.h>

const uint8_t SENSOR_A = 8;   // PCI pin
const uint8_t SENSOR_B = 9;   // PCI pin
const uint8_t SENSOR_C = 2;   // Normal digital input
const uint8_t LED_PIN   = 13;

volatile bool sensorA_changed = false;
volatile bool sensorB_changed = false;
volatile bool timerTick = false;

bool stateA = false;
bool stateB = false;
bool stateC = false;
bool ledState = false;

void setup() {
    Serial.begin(9600);

    pinMode(SENSOR_A, INPUT_PULLUP);
    pinMode(SENSOR_B, INPUT_PULLUP);
	pinMode(SENSOR_C, INPUT);
    pinMode(LED_PIN, OUTPUT);

    PCICR  |= (1 << PCIE0);     // Enable PCI group 0
    PCMSK0 |= (1 << PCINT0);    // D8
    PCMSK0 |= (1 << PCINT1);    // D9

    cli();                      // stop interrupts so we can configure the timers    TCCR1A = 0;
    TCCR1B = 0;

    // CTC mode
    TCCR1B |= (1 << WGM12);

    // OCR1A value for 500 ms (with 1024 prescaler)
    OCR1A = 7812;

    TIMSK1 |= (1 << OCIE1A);

    // Start timer
    TCCR1B |= (1 << CS12) | (1 << CS10);

    sei();                      // start interrupts

    Serial.println("D-LEVEL PCI + TIMER SYSTEM READY");
}

//PCI ISR

ISR(PCINT0_vect) {
    sensorA_changed = true;
    sensorB_changed = true;
}

//Timer 1 ISR

ISR(TIMER1_COMPA_vect) {
    timerTick = true;
}


void readSensors() {
    if (sensorA_changed) {
        stateA = !digitalRead(SENSOR_A);
        sensorA_changed = false;
        Serial.print("A: "); Serial.println(stateA);
    }

    if (sensorB_changed) {
        stateB = !digitalRead(SENSOR_B);
        sensorB_changed = false;
        Serial.print("B: "); Serial.println(stateB);
    }

    bool c = digitalRead(SENSOR_C);
    if (c != stateC) {
        stateC = c;
        Serial.print("C: "); Serial.println(stateC);
    }
}


void processState() {
    int activeCount = stateA + stateB + stateC;

    if (activeCount == 0) {
        digitalWrite(LED_PIN, LOW);
    }
    else if (activeCount == 1) {
        // BLINK LED
        if (timerTick) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
        }
    }
    else {
        // 2 or more sensors active → solid ON
        digitalWrite(LED_PIN, HIGH);
    }
}


void loop() {
    if (timerTick) timerTick = false;

    readSensors();
    processState();
}