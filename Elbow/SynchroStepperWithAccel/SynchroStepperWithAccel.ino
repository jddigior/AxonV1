#include <AccelStepper.h>

// Stepper 5: vertical axis
const int stepPin1 = 2;
const int dirPin1 = 3;

// Stepper 6: horizontal axis
const int stepPin2 = 4;
const int dirPin2 = 5;

// Create stepper instances
AccelStepper stepper5(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper stepper6(AccelStepper::DRIVER, stepPin2, dirPin2);

//Microstep per rev Setting
const int microSteps = 3200;

//Speed acceleration settings
const int maxSpeed = 1000;
const int accRatio = 2;
const int maxAcceleration = accRatio*maxSpeed;

//initialize variables
int targetTime = 0;
int syncSpeed = 0;
int syncAcc = 0;
int step2Go5 = 0;
int step2Go6 = 0;

void setup() {
  Serial.begin(9600);
  stepper5.setMaxSpeed(maxSpeed);
  stepper5.setAcceleration(maxAcceleration);
  stepper6.setMaxSpeed(maxSpeed);
  stepper6.setAcceleration(maxAcceleration);
}

void loop() {
  // Handle serial input
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    double pos5 = input.substring(0, input.indexOf(' ')).toDouble();
    double pos6 = input.substring(input.indexOf(' ') + 1).toDouble();

    move56(pos5, pos6);
  }

  // Run steppers to current target
  stepper5.run();
  stepper6.run();
}

void move56 (double angle5, double angle6){
  int steps5 = int(( angle5/360*microSteps ) + 0.5);
  int steps6 = int(( angle6/360*microSteps ) + 0.5);

  stepper5.moveTo(steps5);
  stepper6.moveTo(steps6);
  Serial.println(steps5);
  Serial.println(steps6);

  step2Go5 = abs(stepper5.distanceToGo());
  step2Go6 = abs(stepper6.distanceToGo());

  //synchro steppers
  if (step2Go5 > step2Go6){
    if(accRatio*step2Go5 >= maxSpeed){
      targetTime = (float)step2Go5 / maxSpeed + 1.0 / accRatio;
      syncSpeed = int((float)maxSpeed * step2Go6 / step2Go5 + 0.5);
      syncAcc = accRatio * syncSpeed;
    } else {
      syncSpeed = maxSpeed;
      syncAcc = int((float)maxAcceleration * step2Go6 / step2Go5 + 0.5);
    }

    stepper5.setMaxSpeed(maxSpeed);
    stepper5.setAcceleration(maxAcceleration);
    stepper6.setMaxSpeed(syncSpeed);
    stepper6.setAcceleration(syncAcc);

  } else if (step2Go6 > step2Go5) {
    if(accRatio*step2Go6 >= maxSpeed){
      targetTime = (float)step2Go6 / maxSpeed + 1.0 / accRatio;
      syncSpeed = int((float)maxSpeed * step2Go5 / step2Go6 + 0.5);
      syncAcc = accRatio * syncSpeed;
    } else {
      syncSpeed = maxSpeed;
      syncAcc = int((float)maxAcceleration * step2Go5 / step2Go6 + 0.5);
    }
    stepper5.setMaxSpeed(syncSpeed);
    stepper5.setAcceleration(syncAcc);
    stepper6.setMaxSpeed(maxSpeed);
    stepper6.setAcceleration(maxAcceleration);

  } else {
    stepper5.setMaxSpeed(maxSpeed);
    stepper5.setAcceleration(maxAcceleration);
    stepper6.setMaxSpeed(maxSpeed);
    stepper6.setAcceleration(maxAcceleration);
  }


  Serial.print("Moving stepper 5 to: ");
  Serial.println(angle5);

  Serial.print("Moving stepper 6 to: ");
  Serial.println(angle6);

}