#include <Servo.h>

// debug mode
#define debugUltrason true

// Pins
#define rightMotorPin1 11
#define rightMotorPin2 10

#define leftMotorPin1 8
#define leftMotorPin2 9

#define colorPin 2
#define jumperPin 3

#define ultraTrig 20
#define ultraEcho 21

#define starterServoPin 19

// Constants
#define rotateTimeLeft 570
#define rotateTimeRight 500

#define minDistanceInCm 20

IntervalTimer ultraTimer;
IntervalTimer motorTimer;

int ultraBuffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ultraCount = 1;
int ultraSum = 0;
boolean hasEnnemy = false;

volatile int loopCounter = 0;
volatile int leftSpeed = 100;
volatile int rightSpeed = 100;

Servo starterServo;

/**
 * Returns true if robot is on green side.
 **/
boolean isGreen() {
  return digitalRead(colorPin);
}

/**
 * Set left motor speed
 * @param int speed - between -100 and 100
 **/
void setLeftSpeed(int speed) {
  leftSpeed = speed + 100;
}

/**
 * Set right motor speed
 * @param int speed - between -100 and 100
 *
 **/
void setRightSpeed(int speed) {
  rightSpeed = speed + 100;
}

/**
 * Stop motors (freewheel)
 **/
void stopMotors() {
  rightSpeed = 100;
  leftSpeed = 100;
}

/**
 * Rotate of +/- 90° to right
 **/
void rotateRight() {
  setLeftSpeed( 60 );
  setRightSpeed( -60 );
  delay(rotateTimeRight);
  stopMotors();
}

/**
 * Rotate of +/- 90° to left
 **/
void rotateLeft() {
  setLeftSpeed( -60 );
  setRightSpeed( 60 );
  delay(rotateTimeLeft);
  stopMotors();
}

/**
 * Go backward
 * @param int speed - between 0 and 100
 **/
void goBackward(int speed) {
  setLeftSpeed(-speed);
  setRightSpeed(-speed);
}

/**
 * Go forward (with ennemy check)
 * @param int speed - between 0 and 100
 * @param int duration - duration in Ms
 **/
void goForward(int speed, int duration) {
  for ( int i = 0; i < duration * 10; i++) {
    if (hasEnnemy) {
      setLeftSpeed(-1); // Break!
      setRightSpeed(-1); // Break!
    } else {
      setLeftSpeed(speed);
      setRightSpeed(speed);
    }
    delay(10);
  }
}

// Servo control methods
/**
 * Do a smooth sploch! (vertical to horizontal)
 **/
void smoothSploch () {
  for (int i = 135; i > 30; i--) {
    starterServo.write(i);
    delay(50);
  }
  starterServo.detach();
}

/**
 * Put the robot in vertical mode
 **/
void prepareToStart () {
  starterServo.attach(starterServoPin);
  starterServo.write(135);
}


void setup() {
  // Motor Right
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // Motor Left
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  motorTimer.begin(_motorsLivecycle, 30);

  // Ultrason
  pinMode(ultraTrig, OUTPUT);
  digitalWrite(ultraTrig, LOW);
  pinMode(ultraEcho, INPUT);

  ultraTimer.begin(_ultrasonLivecycle, 50000);

  // Color switch
  pinMode(colorPin, INPUT);

  // Debug led
  pinMode(13, OUTPUT);

  // Jumper
  pinMode(jumperPin, INPUT);

  // Serial
  Serial.begin(9600);

  if (isGreen()) {
    digitalWrite(13, LOW);
  } else {
    digitalWrite(13, HIGH);
  }
}

void loop() {
  // Waiting for a jumper
  while (!digitalRead(jumperPin));
  prepareToStart();

  // Waiting for remove jumper to start
  while (digitalRead(jumperPin));
  smoothSploch();

  // Calibration on table border
  goBackward(35);
  delay(3000);

  // Forward
  goForward(30, 1800);

  // Break!
  goBackward(1);
  delay(500);
  stopMotors();
  delay(1000);

  // Rotate
  if (isGreen()) {
    rotateLeft();
  } else {
    rotateRight();
  }
  delay(1000);

  // Go upstair!
  goBackward(60);
  delay(900);
  goBackward(1);
  delay(100);
  stopMotors();
}

