#include <Arduino.h>

#define PIN_COM 5      // D1 - Darlington out 3 (Comun)
#define PIN_APRE 4     // D2 - Darlington out 2 (Apertura)
#define PIN_CHIUDE 14  // D5 - Darlington out 1 (Cierre)
#define PIN_FCC 12     // D6 - Final de carrera (Cierre)
#define PIN_FCA 16     // D8 - Final de carrera (Apertura)
#define PIN_FTC 13     // D7 - Control por radio

#define OPEN 0
#define STOP 1
#define CLOSE 2
#define MODE_DEBUG false

// VARIABLES____________________________________________________________________________________________________________
bool advance = false;
byte stateAction = STOP;
// unsigned long time = 0;
// unsigned long timeWorkUpdated = 0;
// unsigned long timeActionUpdate = 0;
// bool isError = false;
// const int TIME_WORK = 22000;
const int DELAY_ACTION = 1500;
const int DELAY_START = 1000;

// MAIN_________________________________________________________________________________________________________________
void setup() {
  delay(DELAY_START);

  // Setting
  Serial.begin(115200);
  pinMode(PIN_COM, OUTPUT);
  pinMode(PIN_APRE, OUTPUT);
  pinMode(PIN_CHIUDE, OUTPUT);
  pinMode(PIN_FCC, INPUT);
  pinMode(PIN_FCA, INPUT);
  pinMode(PIN_FTC, INPUT);

  // Initialize
  // attachInterrupt(PIN_FTC, ChangeState, RISING);
}

void loop() {
  // if (isError) return;

  if (digitalRead(PIN_FTC)) {
    NormalStop();
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

    delay(DELAY_ACTION);

    while (digitalRead(PIN_FTC)) {
      /* Esperar hasta que se deje de precionar el boton del control */
      delay(100);
    }
  }

  switch (stateAction) {
    case OPEN:
      if (!advance) {
        // timeWorkUpdated = time;
        Logger("OPENING");
        advance = true;
      }

      // time = millis();

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
        // timeWorkUpdated = time;
        Logger("CLOSING");
        advance = false;
      }

      // time = millis();

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

// void ChangeState() {
//   if (millis() > DELAY_START && millis() > timeActionUpdate + 2000) {
//     timeActionUpdate = millis();
//     Logger("INTERRUPT");

//     if (advance) {
//       stateAction++;
//     } else {
//       stateAction--;
//     }

//     if (MODE_DEBUG) {
//       if (stateAction == OPEN) Logger("OPEN");
//       if (stateAction == STOP) Logger("STOP");
//       if (stateAction == CLOSE) Logger("CLOSE");
//     }
//   }
// }

// void EmergencyStop() {
//   noInterrupts();
//   NormalStop();
//   isError = true;
//   Logger("ERROR");
// }

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
