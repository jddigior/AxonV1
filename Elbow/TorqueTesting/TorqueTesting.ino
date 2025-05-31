#include <AccelStepper.h>

// Stepper 5: vertical axis
const int stepPin1 = 2;
const int dirPin1 = 3;

// Stepper 6: horizontal axis
const int stepPin2 = 4;
const int dirPin2 = 5;

// Limit Switch Pins
#define limitNO 6
#define limitNC 7

// Create stepper instances
AccelStepper stepper5(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper stepper6(AccelStepper::DRIVER, stepPin2, dirPin2);

// Gear reduction
const int gearReduction = 5;

//Microstep per rev Setting
const int microSteps = 3200;

//Homing variables
int initial_homing = -1;

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

  //initial limit switch find
  stepper5.setMaxSpeed(3);
  stepper5.setAcceleration(500);
  stepper6.setMaxSpeed(3);
  stepper6.setAcceleration(500);
  while (!digitalRead(limitNC)) {  // Make the Stepper move CCW until the switch is activated   
    stepper5.moveTo(initial_homing);  // Set the position to move to
    stepper6.moveTo(initial_homing);
    initial_homing--;  // Decrease by 1 for next move if needed
    stepper5.run();  // Start moving the stepper
    stepper6.run();
    delay(5);
  }
}

void loop() {
}