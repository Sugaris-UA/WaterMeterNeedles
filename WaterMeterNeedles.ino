const int pinsCount = 6;
const int conductionPins[] = { A0, A1, A2, A3, A4, A5 };
const int conductionLeds[] = { 8,  9,  10, 11, 12, 13 };
const int selectorPin = A6;
const int volumePin = A7;
const int relayPin = 2;
const int speakerPin = 3;
const int relayInitialStatePin = 4;
const int stopSignalPin = 5;

const int recheckLimit = 10;
const int conductionThreshold = 20;

int rechecks[] = { 0, 0, 0, 0, 0, 0 };
int endSignalStep = 0;
bool done = false;
bool relayInitialState = false;
int selectorSignal;

void setup() {
  //Serial.begin(115200);

  for (int i = 0; i < pinsCount; i++) {
    pinMode(conductionPins[i], INPUT);
    pinMode(conductionLeds[i], OUTPUT);
  }

  pinMode(relayPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(selectorPin, INPUT);
  pinMode(stopSignalPin, INPUT);
  pinMode(relayInitialStatePin, INPUT);
  pinMode(speakerPin, INPUT);

  relayInitialState = digitalRead(relayInitialStatePin);
  digitalWrite(relayPin, relayInitialState);

  analogWrite(speakerPin, 5);
  delay(200);
  analogWrite(speakerPin, 0);
}

void loop() {
  for (int i = 0; i < pinsCount; i++) {
    int val = analogRead(conductionPins[i]);
    bool waterContacted = val > conductionThreshold;
    bool isSelectedMode = i == getSelectedLevel();

    /*
    if (isSelectedMode) {
      Serial.print('[');
      Serial.print(val, DEC);
      Serial.print(']');
    } else {
      Serial.print(val, DEC);
    }
    Serial.print("\t\t");
    */

    if (waterContacted) {
      digitalWrite(conductionLeds[i], HIGH);

      if (!done) {
        if (isSelectedMode) {
          if (rechecks[i] < recheckLimit) {
            rechecks[i]++;
          } else {
            // selected water level reached
            done = true;
          }
        } else {
          rechecks[i] = 0;
        }
      }
    }
    else {
      rechecks[i] = 0;
      digitalWrite(conductionLeds[i], LOW);
    }
  }
  //Serial.println();

  if (done) {
    digitalWrite(relayPin, !relayInitialState);
    playEndSignal();
  }

  delay(100);
}


int getSelectedLevel() {
  selectorSignal = analogRead(selectorPin);

  if (selectorSignal < 150) {
    return 0;
  } else if (selectorSignal < 300) {
    return 1;
  } else if (selectorSignal < 460) {
    return 2;
  } else if (selectorSignal < 650) {
    return 3;
  } else if (selectorSignal < 830) {
    return 4;
  } else {
    return 5;
  }
}

void playEndSignal() {
  if (isSignalStopped()) {
    analogWrite(speakerPin, 0);
    endSignalStep = 1000;
  }

  if (endSignalStep < 100) {
    endSignalStep++;

    if (endSignalStep == 1) {
      analogWrite(speakerPin, analogRead(volumePin) / 4);
    } else if (endSignalStep == 2) {
      analogWrite(speakerPin, 0);
    } else if (endSignalStep == 3) {
      analogWrite(speakerPin, analogRead(volumePin) / 4);
    } else if (endSignalStep == 4) {
      analogWrite(speakerPin, 0);
    } else if (endSignalStep == 11) {
      endSignalStep = 0;
    }
  }
}

bool isSignalStopped() {
  return digitalRead(stopSignalPin);
}
