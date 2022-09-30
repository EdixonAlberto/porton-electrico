#define PIN_COM 7     // Darlington out 3 (Comun)
#define PIN_APRE 6    // Darlington out 2 (Apertura)
#define PIN_CHIUDE 5  // Darlington out 1 (Cierre)
#define PIN_FCC 9     // Final de carrera (Cierre)
#define PIN_FCA 8     // Final de carrera (Apertura)
#define PIN_CONTROL 4

#define OPEN 0
#define STOP 1
#define CLOSE 2

bool advance = false;
volatile byte stateAction = STOP;
unsigned long time = 0;
unsigned long timeUpdated = 0;
bool isError = false;
const int TIME_WORK = 5000;
const int DELAY_WORK = 1000;

void setup() {
  // CONFIG
  Serial.begin(9600);
  pinMode(PIN_COM, OUTPUT);
  pinMode(PIN_APRE, OUTPUT);
  pinMode(PIN_CHIUDE, OUTPUT);
  pinMode(PIN_CONTROL, INPUT);

  // INITIAL
  digitalWrite(PIN_COM, LOW);
  digitalWrite(PIN_APRE, LOW);
  digitalWrite(PIN_CHIUDE, LOW);

  // INTERRUPTS
  attachInterrupt(0, ChangeState, RISING);
}

void loop() {
  if (isError) return;

  switch (stateAction) {
    case OPEN:
      if (!advance) {
        delay(DELAY_WORK);
        timeUpdated = time;
      }

      time = millis();
      advance = true;

      if (digitalRead(PIN_FCA)) {
        stateAction = STOP;
        Serial.println("STOP");
      } else {
        digitalWrite(PIN_COM, 1);
        digitalWrite(PIN_APRE, 1);
        digitalWrite(PIN_CHIUDE, 0);
        Serial.println("OPENING");
      }

      if (time > timeUpdated + TIME_WORK && !digitalRead(PIN_FCA)) {
        EmergencyStop();
      }
      break;

    case STOP:
      digitalWrite(PIN_COM, 0);
      digitalWrite(PIN_APRE, 0);
      digitalWrite(PIN_CHIUDE, 0);
      break;

    case CLOSE:
      if (advance) {
        delay(DELAY_WORK);
        timeUpdated = time;
      }

      time = millis();
      advance = false;

      if (digitalRead(PIN_FCC)) {
        stateAction = STOP;
        Serial.println("STOP");
      } else {
        digitalWrite(PIN_COM, 1);
        digitalWrite(PIN_APRE, 0);
        digitalWrite(PIN_CHIUDE, 1);
        Serial.println("CLOSING");
      }

      if (time > timeUpdated + TIME_WORK && !digitalRead(PIN_FCC)) {
        EmergencyStop();
      }
      break;
  }
}

void ChangeState() {
  if (advance)
    stateAction++;
  else
    stateAction--;

  if (stateAction == OPEN) Serial.println("OPEN");
  if (stateAction == STOP) Serial.println("STOP");
  if (stateAction == CLOSE) Serial.println("CLOSE");
}

void EmergencyStop() {
  digitalWrite(PIN_COM, 0);
  digitalWrite(PIN_APRE, 0);
  digitalWrite(PIN_CHIUDE, 0);
  isError = true;
  stateAction = STOP;
  time = 0;
  timeUpdated = 0;
  Serial.println("ERROR");
}