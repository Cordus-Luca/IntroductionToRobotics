//DS= [D]ata [S]torage - data
//STCP= [ST]orage [C]lock [P]in latch
//SHCP= [SH]ift register [C]lock [P]in clock

const int pinSW = 2;  // digital pin connected to the switch output
const int pinX = A0;  // A0 - analog pin connected to X output
const int pinY = A1;  // A1 - analog pin connected to Y output

const int latchPin = 11;  // STCP to 12 on Shift Register
const int clockPin = 10;  // SHCP to 11 on Shift Register
const int dataPin = 12;   // DS to 14 on Shift Register

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;


const int blinkInterval = 400;
const long resetTime = 3000;
unsigned long previousMillis = 0;
unsigned long pressedDebounceTime = 0;
unsigned long releasedDebounceTime = 0;
unsigned long start = 0;
unsigned long pressTime = 0;
unsigned long releaseTime = 0;

int minThreshold = 400;
int maxThreshold = 600;

int currentState = 1;

const byte regSize = 8;  // 1 byte aka 8 bits

byte blinkIndex = 1;
byte xJoyMoved = false;
byte yJoyMoved = false;
byte lastSwState = LOW;
byte swState = LOW;
byte reading = HIGH;
byte state = 1;
byte dpState = LOW;
byte pressed = 0;


unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;

bool buttonPressed = true;
bool shortPress = false;

int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

int displayNumber[] = {
  0, 0, 0, 0
};

const int encodingsNumber = 16;

int byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  //B00000001, // DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};

const int displayCount = 4;

int registers[regSize];

void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  Serial.begin(9600);
}

int index = 0;

void loop() {

  firstState();

  swState = digitalRead(pinSW);

  buttonPress();
  showDigits(index);
}

void firstState() {

  int xValue = analogRead(pinX);

  if (currentState == 1) {
    if (xJoyMoved == false) {
      if (xValue > maxThreshold) {
        xJoyMoved = true;
        if (index > 0) {
          index--;
        }
      } else {
        if (xValue < minThreshold) {
          xJoyMoved = true;
          if (index < 3) {
            index++;
          }
        }
      }
    }

    if (minThreshold <= xValue && xValue <= maxThreshold) {
      xJoyMoved = false;
    }
  }
}

void secondState() {
  int yValue = analogRead(pinY);

  if (yJoyMoved == false) {
    if (yValue > maxThreshold) {
      yJoyMoved = true;
      if (displayNumber[index] > 0) {
        displayNumber[index]--;
      }
    } else {
      if (yValue < minThreshold) {
        yJoyMoved = true;
        if (displayNumber[index] < 15) {
          displayNumber[index]++;
        }
      }
    }
  }

  if (minThreshold <= yValue && yValue <= maxThreshold) {
    yJoyMoved = false;
  }
}

void showDigits(int index) {
  unsigned long currentMillis = millis();

  for (int i = 0; i < 4; ++i) {
    int digit = 0;
    if (i == index) {
      if (currentState == 2) {
        digit = byteEncodings[displayNumber[index]] + 1;
      } else {
        digit = byteEncodings[displayNumber[index]];

        if (millis() - previousMillis > blinkInterval) {
          digit = byteEncodings[displayNumber[index]] + blinkIndex;
        }
        if (millis() - previousMillis > 2 * blinkInterval) {
          digit = byteEncodings[displayNumber[index]];
          previousMillis = millis();
        }
      }
    } else {
      digit = byteEncodings[displayNumber[i]];
    }

    writeReg(digit);
    activateDisplay(i);
    delay(5);
  }
}

void buttonPress() {

  if (swState != lastSwState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (swState != reading) {
      reading = swState;

      if (reading == LOW) {
        pressTime = millis();
      } else {
        releaseTime = millis();
        pressed = 0;
      }

      long pressDuration = releaseTime - pressTime;

      if (pressDuration > resetTime && currentState == 1) {
        reset();
      } else {
        if (pressDuration > debounceDelay && pressed == 0) {
          if (currentState == 2) {
            currentState = 1;
            pressed = 1;
          } else {
            currentState = 2;
            pressed = 1;
          }
          Serial.println(currentState);
        }
      }
    }
  }


  if (currentState == 2) {
    secondState();
  }

  lastSwState = swState;
}

void activateDisplay(int digit) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  digitalWrite(displayDigits[digit], LOW);
}

void writeReg(int encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

void reset() {
  for (int i = 0; i < 4; i++) {
    displayNumber[i] = 0;
  }

  index = 0;
}
