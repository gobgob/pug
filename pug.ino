#include <Servo.h>

// debug mode
#define debugUltrason false

// Pins
#define RIGHT_MOTOR_PIN_1 11
#define RIGHT_MOTOR_PIN_2 10

#define LEFT_MOTOR_PIN_1 8
#define LEFT_MOTOR_PIN_2 9

#define COLOR_ANALOG_PIN 1
#define JUMPER_PIN 3

#define ENEMY_LED_PIN 13
#define GREEN_LED_PIN 7
#define YELLOW_LED_PIN 6

#define STARTER_SERVO_PIN 19

// Constants
#define ROTATE_TIME_LEFT 570
#define ROTATE_TIME_RIGHT 500

#define MIN_DISTANCE_IN_CM 20


#define ULTRASOUND true

IntervalTimer ultraTimer;
IntervalTimer motorTimer;

volatile unsigned int ultraBuffer[3] = {0, 0, 0};
volatile unsigned int ultraCount = 1;
volatile unsigned int ultraSum = 0;
volatile boolean hasEnnemy = false;

volatile int loopCounter = 0;
volatile int leftSpeed = 100;
volatile int rightSpeed = 100;

Servo starterServo;

/**
 * Returns true if robot is on green side.
 **/
boolean isGreen() {
  return analogRead(COLOR_ANALOG_PIN) > 500;
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
  delay(ROTATE_TIME_RIGHT);
  stopMotors();
}

/**
 * Rotate of +/- 90° to left
 **/
void rotateLeft() {
  setLeftSpeed( -60 );
  setRightSpeed( 60 );
  delay(ROTATE_TIME_LEFT);
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
  for ( int i = 0; i < duration / 1; i++) {
    if (ULTRASOUND && hasEnnemy) {
      setLeftSpeed(-1); // Break!
      setRightSpeed(-1); // Break!
      i--;
    } else {
      setLeftSpeed(speed);
      setRightSpeed(speed);
    }
    delay(1);
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
  starterServo.attach(STARTER_SERVO_PIN);
  starterServo.write(135);
}


void setup() {
  // Motor Right
  pinMode(LEFT_MOTOR_PIN_1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN_2, OUTPUT);

  // Motor Left
  pinMode(RIGHT_MOTOR_PIN_1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_2, OUTPUT);

  motorTimer.begin(_motorsLivecycle, 10);
  ultraTimer.begin(_ultrasonLivecycle, 25000);

  // Debug led
  pinMode(ENEMY_LED_PIN, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);

  // Jumper
  pinMode(JUMPER_PIN, INPUT);

  // Serial
  Serial.begin(9600);

  // Highlight leds in function of robot color
  digitalWrite(GREEN_LED_PIN, isGreen());
  digitalWrite(YELLOW_LED_PIN, !isGreen());
}

void loop() {

// while (42){
//   Serial.println(analogRead(COLOR_ANALOG_PIN));
//   Serial.println(isGreen());
// }

  // Waiting for a jumper
  while (!digitalRead(JUMPER_PIN));
  prepareToStart();

  // Waiting for remove jumper to start
  while (digitalRead(JUMPER_PIN));
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

