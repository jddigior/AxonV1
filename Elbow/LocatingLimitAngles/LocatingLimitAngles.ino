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
int moveCounter = 0;


void setup() {
  Serial.begin(9600);
  // Enable pullups on limit switch pins
  pinMode(limitNO, INPUT_PULLUP);
  pinMode(limitNC, INPUT_PULLUP);

  //initial limit switch find
  stepper5.setMaxSpeed(1000);
  stepper5.setAcceleration(1000);
  stepper6.setMaxSpeed(1000);
  stepper6.setAcceleration(1000);
  Serial.println("Steppers 5 and 6 are homing . . .");
  while (!digitalRead(limitNC)) {  // Make the Stepper move CCW until the switch is activated   
    stepper5.moveTo(initial_homing);  // Set the position to move to
    stepper6.moveTo(-initial_homing);
    initial_homing--;  // Decrease by 1 for next move if needed
    stepper5.run();  // Start moving the stepper
    stepper6.run();
    delay(5);
  }
  stepper5.setCurrentPosition(0);
  stepper6.setCurrentPosition(0);  // Set the current position as zero for now

  //reverse
  initial_homing = 1;
  stepper5.setMaxSpeed(500);
  stepper5.setAcceleration(500);
  stepper6.setMaxSpeed(500);
  stepper6.setAcceleration(500);
  while (digitalRead(limitNC)) {   
    stepper5.moveTo(initial_homing);  // Set the position to move to
    stepper6.moveTo(-initial_homing);
    initial_homing++;  
    stepper5.run();  // Start moving the stepper
    stepper6.run();
    delay(5);
  }
  delay(500);

  //back into switch very slowly
  stepper5.setMaxSpeed(100);
  stepper5.setAcceleration(50);
  stepper6.setMaxSpeed(100);
  stepper6.setAcceleration(50);
  while (!digitalRead(limitNC)) {  // Make the Stepper move CCW until the switch is activated   
    stepper5.moveTo(initial_homing);  // Set the position to move to
    stepper6.moveTo(-initial_homing);
    initial_homing--;  // Decrease by 1 for next move if needed
    stepper5.run();  // Start moving the stepper
    stepper6.run();
    delay(5);
  }
  stepper5.setCurrentPosition(0);
  stepper6.setCurrentPosition(0);  // Set the current position as zero for now

  //reverse and spin slightly
  moveBC(5, 0);
  while (stepper5.distanceToGo()!=0 || stepper6.distanceToGo()!=0) {
    stepper5.run();
    stepper6.run();
  }
  delay(250);

  int reverseSteps = stepper5.currentPosition();
  moveBC(5, 45);
  while (abs(stepper5.distanceToGo())>0 || abs(stepper6.distanceToGo())>0) {
    stepper5.run();
    stepper6.run();
  }
  stepper5.setCurrentPosition(reverseSteps);
  stepper6.setCurrentPosition(-reverseSteps);
  delay(500);

  //back into switch very slowly
  stepper5.setMaxSpeed(100);
  stepper5.setAcceleration(50);
  stepper6.setMaxSpeed(100);
  stepper6.setAcceleration(50);
  initial_homing = reverseSteps -1;
  while (!digitalRead(limitNC)) {  // Make the Stepper move CCW until the switch is activated   
    stepper5.moveTo(initial_homing);  // Set the position to move to
    stepper6.moveTo(-initial_homing);
    initial_homing--;  // Decrease by 1 for next move if needed
    stepper5.run();  // Start moving the stepper
    stepper6.run();
    delay(5);
  }

/*
  if (stepper5.currentPosition() > 0){
    stepper5.setCurrentPosition(0);
    stepper6.setCurrentPosition(0);
  } else {
    moveBC(0, -45);
    while(stepper5.distanceToGo() != 0 || stepper6.distanceToGo() != 0){
      stepper5.run();
      stepper6.run();
    }

  }
*/

  //Axis C homing
  stepper5.setMaxSpeed(800);
  stepper5.setAcceleration(800);
  stepper6.setMaxSpeed(800);
  stepper6.setAcceleration(800);
  initial_homing = -1;
  while (digitalRead(limitNC)) { 
    stepper5.moveTo(initial_homing);  // Set the position to move to
    stepper6.moveTo(initial_homing);
    initial_homing--;  // Decrease by 1 for next move if needed
    stepper5.run();  // Start moving the stepper
    stepper6.run();
    delay(5);
  }
  stepper5.setCurrentPosition(0);
  stepper6.setCurrentPosition(0);  
  stepper5.setMaxSpeed(maxSpeed);
  stepper5.setAcceleration(maxAcceleration);
  stepper6.setMaxSpeed(maxSpeed);
  stepper6.setAcceleration(maxAcceleration);

  moveBC(92, 87.75); //Tune this to limit switch location
  while (stepper5.distanceToGo()!=0 || stepper6.distanceToGo()!=0) {
    stepper5.run();
    stepper6.run();
  }
  stepper5.setCurrentPosition(0);
  stepper6.setCurrentPosition(0);  
  stepper5.setMaxSpeed(maxSpeed);
  stepper5.setAcceleration(maxAcceleration);
  stepper6.setMaxSpeed(maxSpeed);
  stepper6.setAcceleration(maxAcceleration);

  Serial.println("Ready. Use command format: angleB angleC");
}

void loop(){

}

void moveBC (double angleB, double angleC){
  double angle5 = gearReduction*(angleB + angleC);
  double angle6 = gearReduction*(-angleB + angleC);

  int steps5 = int(( angle5/360*microSteps ) + 0.5);
  int steps6 = int(( angle6/360*microSteps ) + 0.5);

  stepper5.moveTo(steps5);
  stepper6.moveTo(steps6);

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