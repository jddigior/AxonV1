// Pin definitions
const int joyX = A0;
const int joyY = A1;

const int stepPin1 = 2;
const int dirPin1 = 3;
const int stepPin2 = 4;
const int dirPin2 = 5;

const int limitNC = 7;  // Normally Closed
const int limitNO = 6;  // Normally Open

// Tuning parameters
const int deadzone = 100;
const int center = 512;
const int maxDelay = 1000;
const int minDelay = 20;

void setup() {
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);

  pinMode(limitNC, INPUT_PULLUP);
  pinMode(limitNO, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  int xVal = analogRead(joyX);
  int yVal = analogRead(joyY);

  // Read raw pin values
  int stateNC = digitalRead(limitNC);
  int stateNO = digitalRead(limitNO);

  // Display states
  Serial.print("Pin 7 (NC): ");
  Serial.print(stateNC);
  Serial.print(" | Pin 6 (NO): ");
  Serial.println(stateNO);

  // Stepper 1 control
  if (abs(yVal - center) > deadzone) {
    digitalWrite(dirPin1, yVal > center ? HIGH : LOW);
    int speed = map(abs(yVal - center), 0, 512, maxDelay, minDelay);
    digitalWrite(stepPin1, HIGH);
    delayMicroseconds(speed);
    digitalWrite(stepPin1, LOW);
    delayMicroseconds(speed);
  }

  // Stepper 2 control
  if (abs(xVal - center) > deadzone) {
    digitalWrite(dirPin2, xVal > center ? HIGH : LOW);
    int speed = map(abs(xVal - center), 0, 512, maxDelay, minDelay);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(speed);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(speed);
  }

  delay(5); // Slow down serial output for readability
}
