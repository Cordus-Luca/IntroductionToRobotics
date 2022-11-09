const int baudRate = 9600;

// declare all the joystick pins
const int pinSW = 2;  // digital pin connected to switch output
const int pinX = A0;  // A0 - analog pin connected to X output
const int pinY = A1;  // A1 - analog pin connected to Y output

// declare all the segments pins
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int segSize = 8;

byte state = HIGH;
byte dpState = LOW;
byte swState = HIGH;
byte lastRepeatState = HIGH;
byte currentRepeatState = HIGH;

byte lastSwState = LOW;
int xValue = 0;
int yValue = 0;

bool repeated = false;
bool secondStateSw = false;
bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;
int direction = 0;
int aux = 0;

int currentLocation = 11;
int prevLocation = 0;
byte currentLocationState = LOW;
byte prevLocationState = LOW;
byte seqStart = 0;
byte lastState;

const int blinkInterval = 350;
const int repeatTime = 3000;

unsigned long previousMillis = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 150;
long pressedTime;
long releasedTime;

long pressDuration = 0;

byte pinStates[segSize] = {
  LOW, LOW, LOW, LOW, LOW, LOW, LOW
};

bool secondStateMatrix[segSize] = {
  false, false, false, false, false, false, false  // {A, B, C, D, E, F, G}
};

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

// {UP, DOWN, LEFT, RIGHT}
const int movementMatrix[8][4] = {
  { 0, 10, 9, 5 },   // A
  { 4, 10, 9, 0 },   // B
  { 10, 7, 8, 11 },  // C
  { 10, 0, 8, 6 },   // D
  { 10, 7, 0, 6 },   // E
  { 4, 10, 0, 5 },   // F
  { 4, 7, 0, 0 },    // G
  { 0, 0, 6, 0 }     // DP
};

void setup() {
  // initialize all the pins
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);

  Serial.begin(baudRate);
}

void loop() {

  int reading = digitalRead(pinSW);
  
  /*int currentState = digitalRead(pinSW);

  if (lastState == HIGH && currentState == LOW) {
    pressedTime = millis();
  } else if (lastState = LOW && currentState == HIGH && (pressedTime - millis()) > debounceDelay) {
    releasedTime = millis();
  } 

//buttonPress();

  if (repeated == false){
    if (releasedTime - pressedTime > repeatTime) {
      Serial.println(releasedTime - pressedTime);
      Serial.println(releasedTime);
      Serial.println(pressedTime);
      currentLocation = 11;
      for (int i = 0; i < segSize; i++) {
        pinStates[i] = LOW;
        secondStateMatrix[i] = false;
      }
      blink();
      repeated = true;
    }
  }
*/
  if (reading != lastSwState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != swState) {
      swState = reading;

      if (swState == 0) {
        seqStart = 1;
      }
    }
  }
  
  sequence();

  //lastState = currentState;
  lastSwState = reading;
}

/*
void buttonPress() {
  int currentState = digitalRead(pinSW);

  if (lastState == HIGH && currentState == LOW) {
    pressedTime = millis();
  } else if (lastState = LOW && currentState == HIGH) {
    releasedTime = millis();
  }
}

void reset() {
  currentLocation = 11;
  for (int i = 0; i < segSize; i++) {
    pinStates[i] = LOW;
    secondStateMatrix[i] = false;
  }
}
*/

void sequence() {
  int reading = digitalRead(pinSW);
  int index = currentLocation - 4;

  if (seqStart) {
    secondStateMatrix[index] = true;

    pinStates[index] = currentLocationState;
    int xValue = analogRead(pinX);

    digitalWrite(segments[index], pinStates[index]);

    if (reading != lastSwState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {

      if (reading != swState) {
        swState = reading;

        if (swState == 0) {
          seqStart = 0;
        }
      }
    }

  } else {
    aux = pinStates[index];
    secondStateMatrix[index] = false;
    digitalWrite(currentLocation, currentLocationState);
    blink();
    directionFunc();
  }

  for (int i = 0; i < segSize; i++) {
    if (currentLocationState == 0 && index == i) {
      digitalWrite(segments[i], 0);
    } else {
      digitalWrite(segments[i], pinStates[i]);
    }
  }

  repeated = false;
}

void blink() {
  unsigned long currentMillis = millis();
  int index = currentLocation - 4;

  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    currentLocationState = !currentLocationState;
    digitalWrite(segments[index], !currentLocationState);
  }
}

void directionFunc() {
  int xValue, yValue;

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (minThreshold <= xValue && xValue <= maxThreshold && minThreshold <= yValue && yValue <= maxThreshold) {
    joyMoved = false;
  }

  if (joyMoved == false) {
    if (xValue > maxThreshold) {
      joyMoved = true;
      direction = 3;  // RIGHT
    } else {
      if (xValue < minThreshold) {
        joyMoved = true;
        direction = 2;  // LEFT
      } else {
        if (yValue < minThreshold) {
          joyMoved = true;
          direction = 0;  // UP
        } else {
          if (yValue > maxThreshold) {
            joyMoved = true;
            direction = 1;  // DOWN
          }
        }
      }
    }
    if (movementMatrix[currentLocation - 4][direction] != 0 && joyMoved == true) {
      currentLocation = movementMatrix[currentLocation - 4][direction];
    }
  }
}