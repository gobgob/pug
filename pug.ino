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

// # Low level controls
void _backwardRight() {
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

void _forwardRight() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}

void _stopRight() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
}

// ## Motor Left
void _backwardLeft() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
}

void _forwardLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
}

void _stopLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
}

long ultraMesure() {
  long lecture_echo;
  long cm;

  digitalWrite(ultraTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraTrig, LOW);
  lecture_echo = pulseIn(ultraEcho, HIGH, 500000);
  cm = lecture_echo / 29;

  return cm;
}

/**
 * Motors livecycle
 **/
void _motorsLivecycle()
{
  loopCounter = (loopCounter + 1) % 200;

  if (rightSpeed == 100) _stopRight();
  else if (loopCounter >= rightSpeed)_forwardRight();
  else _backwardRight();

  if (leftSpeed == 100) _stopLeft();
  else if (loopCounter >= leftSpeed)_forwardLeft();
  else _backwardLeft();

}

/**
 * Ultrason livecycle
 **/
void _ultrasonLivecycle() {
  int i = ultraCount % 10;
  int j = (ultraCount + 1) % 10;
  ultraBuffer[i] = ultraMesure();
  ultraSum += ultraBuffer[i];
  ultraSum -= ultraBuffer[j];
  if (debugUltrason) Serial.println(ultraSum / 10);

  hasEnnemy = (ultraSum / 10 < minDistanceInCm);
  ultraCount++;
}

/**
 * Returns true if robot is on green side.
 **/
boolean isGreen() {
  return digitalRead(colorPin);
}

// Motors controls methods

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

// Serial
char serialInput[100];
int serialPrompt = 0;

void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();

    switch (inChar)
    {
      case '0'...'9':
        serialInput[serialPrompt] = inChar - '0';
        serialPrompt++;
        break;

      case '\n':
        serialExecute();
        break;

      default:
        serialInput[serialPrompt] = inChar;
        serialPrompt++;
        break;
    }
  }
}


void serialExecute()
{
  switch (serialInput[0])
  {
    case 'r':
    case 'R': // Rotation
      if (serialInput[1] == 'L' || serialInput[1] == 'l') {
        rotateLeft();
        Serial.println("Step left!");
      }
      else {
        rotateRight();
        Serial.println("Step right!");
      }
      break;

    case 'f':
    case 'F': // Front direction
      Serial.println("Go!");
      setLeftSpeed( serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]);
      setRightSpeed( serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]);
      delay(1500);
      setLeftSpeed( -2 );
      setRightSpeed( -2);
      delay(400);
      stopMotors();
      break;

    case 'b':
    case 'B': // Back direction
      Serial.println("Revert revert revert!");
      setLeftSpeed( (serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]) * -1);
      setRightSpeed( (serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]) * -1);
      break;

    case 's':
    case 'S': // Starting block
      Serial.println("Lock'n'load!");
      prepareToStart();
      break;

    case 'g':
    case 'G': // GO !!!
      Serial.println("Smooth splosh");
      smoothSploch();
      break;

    case 'W':
    case 'w': // Emergy stop
      Serial.println("Emergency Stop!!");
      stopMotors();
      break;

    case 'd':
    case 'D':
      Serial.println("Right on");
      setRightSpeed(30);
      break;

    case 'l':
    case 'L':
      Serial.println("left on");
      setLeftSpeed(30);
      break;

    default:
      Serial.print("Unkwnown command (");
      Serial.print(serialInput[0]);
      Serial.println(")");
  }

  // Reset var for next instruction
  for (int i = 0; i < 10; i++)
    serialInput[i] = 0;
  serialPrompt = 0;
}
