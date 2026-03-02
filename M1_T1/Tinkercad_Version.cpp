const uint8_t SENSOR_A = 8;   // Simulated PCI pin
const uint8_t SENSOR_B = 9;   // Simulated PCI pin
const uint8_t SENSOR_C = 2;   // Normal input
const uint8_t LED_PIN   = 13;

// Saved states
bool stateA = false;
bool stateB = false;
bool stateC = false;

// For simulated timer
unsigned long lastTimer = 0;
const unsigned long TIMER_INTERVAL = 500;  // 500ms

void setup() {
    Serial.begin(9600);

    pinMode(SENSOR_A, INPUT_PULLUP);
    pinMode(SENSOR_B, INPUT_PULLUP);
    pinMode(SENSOR_C, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    Serial.println("TINKERCAD VERSION READY");
}

void readSensors() {
    bool a = !digitalRead(SENSOR_A);
    bool b = !digitalRead(SENSOR_B);
    bool c = !digitalRead(SENSOR_C);

    if (a != stateA) {
        stateA = a;
        Serial.print("Sensor A changed -> ");
        Serial.println(stateA);
    }

    if (b != stateB) {
        stateB = b;
        Serial.print("Sensor B changed -> ");
        Serial.println(stateB);
    }

    if (c != stateC) {
        stateC = c;
        Serial.print("Sensor C changed -> ");
        Serial.println(stateC);
    }
}

void processState() {
    int activeCount = stateA + stateB + stateC;

    if (activeCount >= 2) {
        digitalWrite(LED_PIN, HIGH);  // Solid ON
    }
    else if (activeCount == 1) {
        // Blink LED using simulated timer
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    else {
        digitalWrite(LED_PIN, LOW);
    }
}

-------------------------------------------------
void loop() {
    if (millis() - lastTimer >= TIMER_INTERVAL) { //simluating timer interrupt
        lastTimer = millis();
        Serial.println("Timer event (simulated)");
        processState();
    }
  
    readSensors();
}
