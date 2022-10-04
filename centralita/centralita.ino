#define PIN_COM 7     // Darlington out 3 (Comun)
#define PIN_APRE 6    // Darlington out 2 (Apertura)
#define PIN_CHIUDE 5  // Darlington out 1 (Cierre)
#define PIN_FCC 9     // Final de carrera (Cierre)
#define PIN_FCA 8     // Final de carrera (Apertura)
#define PIN_CONTROL 4

#define OPEN 0
#define STOP 1
#define CLOSE 2
#define MODE_DEBUG false

// VARIABLES____________________________________________________________________________________________________________
volatile bool advance = false;
volatile byte stateAction = STOP;
unsigned long time = 0;
unsigned long timeWorkUpdated = 0;
unsigned long timeActionUpdate = 0;
bool isError = false;
const int TIME_WORK = 22000;
const int DELAY_ACTION = 1000;
const int DELAY_START = 1000;

// MAIN_________________________________________________________________________________________________________________
void setup() {
  // CONFIG
  Serial.begin(9600);
  pinMode(PIN_COM, OUTPUT);
  pinMode(PIN_APRE, OUTPUT);
  pinMode(PIN_CHIUDE, OUTPUT);
  pinMode(PIN_FCC, INPUT);
  pinMode(PIN_FCA, INPUT);
  pinMode(PIN_CONTROL, INPUT);

  // START
  NormalStop();
  attachInterrupt(0, ChangeState, RISING);
}

void loop() {
  if (isError) return;

  switch (stateAction) {
    case OPEN:
      if (!advance) {
        delay(DELAY_ACTION);
        timeWorkUpdated = time;
        Logger("OPENING");
      }

      time = millis();
      advance = true;

      if (digitalRead(PIN_FCA)) {
        digitalWrite(PIN_COM, 1);
        digitalWrite(PIN_APRE, 1);
        digitalWrite(PIN_CHIUDE, 0);
      } else {
        stateAction = STOP;
        Logger("STOP");
      }

      // if (time > timeWorkUpdated + TIME_WORK && digitalRead(PIN_FCA)) {
      //   EmergencyStop();
      // }
      break;

    case STOP:
      NormalStop();
      break;

    case CLOSE:
      if (advance) {
        delay(DELAY_ACTION);
        timeWorkUpdated = time;
        Logger("CLOSING");
      }

      time = millis();
      advance = false;

      if (digitalRead(PIN_FCC)) {
        digitalWrite(PIN_COM, 1);
        digitalWrite(PIN_APRE, 0);
        digitalWrite(PIN_CHIUDE, 1);
      } else {
        stateAction = STOP;
        Logger("STOP");
      }

      // if (time > timeWorkUpdated + TIME_WORK && digitalRead(PIN_FCC)) {
      //   EmergencyStop();
      // }
      break;
  }
}

// METHODS______________________________________________________________________________________________________________
void ChangeState() {
  if (millis() > DELAY_START && millis() > timeActionUpdate + 250) {
    if (stateAction == STOP ||
        millis() > timeActionUpdate + DELAY_ACTION + 250) {
      Logger("INTERRUPT");
      if (advance) {
        stateAction++;
      } else {
        stateAction--;
      }

      if (MODE_DEBUG) {
        if (stateAction == OPEN) Logger("OPEN");
        if (stateAction == STOP) Logger("STOP");
        if (stateAction == CLOSE) Logger("CLOSE");
      }

      timeActionUpdate = millis();
    }
  }
}

void EmergencyStop() {
  noInterrupts();
  NormalStop();
  isError = true;
  Logger("ERROR");
}

void NormalStop() {
  digitalWrite(PIN_COM, 0);
  digitalWrite(PIN_APRE, 0);
  digitalWrite(PIN_CHIUDE, 0);
}

void Logger(String message) {
  if (MODE_DEBUG) {
    Serial.println(message);
  }
}
