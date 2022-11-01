const int buttonPin = 2;
const int buzzerPin = 8;
const int pedestrianGreenLedPin = 9;
const int pedestrianRedLedPin = 10;
const int carGreenLedPin = 11;
const int carYellowLedPin = 12;
const int carRedLedPin = 13;
const int baudRate = 9600;

byte buttonState = 0;
byte pedestrianGreenLedState = 0;
byte pedestrianRedLedState = 1;
byte carGreenLedState = 1;
byte carYellowLedState = 0;
byte carRedLedState = 0;
byte lastButtonState = 0;
byte buzzerState = 0;
byte seqStart = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long startingDelay = 8000;
unsigned long yellowLedPinDelay = startingDelay + 3000;
unsigned long firstPedestrianGreenLedBlinkDelay = yellowLedPinDelay + 8000;
unsigned long secondPedestrianGreenLedBlinkDelay = firstPedestrianGreenLedBlinkDelay + 4000;


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pedestrianGreenLedPin, OUTPUT);
  pinMode(pedestrianRedLedPin, OUTPUT);
  pinMode(carGreenLedPin, OUTPUT);
  pinMode(carYellowLedPin, OUTPUT);
  pinMode(carRedLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(baudRate);
}

void loop() {
  // read the state of the switch into a local variable
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState = reading;

    if (buttonState == 0) {
      seqStart = 1;
    }

    if (seqStart) {
      if ((millis() - lastDebounceTime) > startingDelay) {
          carGreenLedState = 0;
          carYellowLedState = 1 ;
      }

      if ((millis() - lastDebounceTime) > yellowLedPinDelay) {
        carYellowLedState = 0;
        carRedLedState = 1;
        pedestrianGreenLedState = 1;
        pedestrianRedLedState = 0;

        digitalWrite(carYellowLedPin, carYellowLedState);
        digitalWrite(carRedLedPin, carRedLedState);
        digitalWrite(pedestrianGreenLedPin, pedestrianGreenLedState);
        digitalWrite(pedestrianRedLedPin, pedestrianRedLedState);

        if ((millis() - lastDebounceTime) <= firstPedestrianGreenLedBlinkDelay) {
          buzzerState = !buzzerState;

          if (buzzerState) {
            tone(buzzerPin, 450);
          }
          
          Serial.println(buzzerState);
          delay(500);
          noTone(buzzerPin);
        }
      }
      
      if ((millis() - lastDebounceTime) > firstPedestrianGreenLedBlinkDelay) {
        for (int i = 0; i<= 19; i++) {
          buzzerState = !buzzerState;
          pedestrianGreenLedState = !buzzerState;
          digitalWrite(pedestrianGreenLedPin, pedestrianGreenLedState);

          if (buzzerState) {
            tone(buzzerPin, 350);
          }

          delay(200);
          noTone(buzzerPin);
          Serial.println(buzzerState);
        }
        seqStart = 0;
      }
      
      if (seqStart == 0) {
        pedestrianGreenLedState = 0;
        pedestrianRedLedState = 1;
        carGreenLedState = 1;
        carYellowLedState = 0;
        carRedLedState = 0;
        lastButtonState = 0;
      }
    }
  }

  digitalWrite(pedestrianGreenLedPin, pedestrianGreenLedState);
  digitalWrite(pedestrianRedLedPin, pedestrianRedLedState);
  digitalWrite(carGreenLedPin, carGreenLedState);
  digitalWrite(carYellowLedPin, carYellowLedState);
  digitalWrite(carRedLedPin, carRedLedState);

  lastButtonState = reading;
}
