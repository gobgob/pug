#include <NewPing.h>

#define TRIGGER_PIN  20  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     21  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

/**
 * Set right motor in backward position (low level)
 * @private
 **/
void _backwardRight() {
  digitalWrite(RIGHT_MOTOR_PIN_1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN_2, LOW);
}


/**
 * Set right motor in forward position (low level)
 * @private
 **/
void _forwardRight() {
  digitalWrite(RIGHT_MOTOR_PIN_1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_2, HIGH);
}

/**
 * Stop right motor (freewheel)
 * @private
 **/
void _stopRight() {
  digitalWrite(RIGHT_MOTOR_PIN_1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_2, LOW);
}

/**
 * Set left motor in backward position (low level)
 * @private
 **/
void _backwardLeft() {
  digitalWrite(LEFT_MOTOR_PIN_1, HIGH);
  digitalWrite(LEFT_MOTOR_PIN_2, LOW);
}

/**
 * Set left motor in forward position (low level)
 * @private
 **/
void _forwardLeft() {
  digitalWrite(LEFT_MOTOR_PIN_1, LOW);
  digitalWrite(LEFT_MOTOR_PIN_2, HIGH);
}

/**
 * Stop left motor (freewheel)
 * @private
 **/
void _stopLeft() {
  digitalWrite(LEFT_MOTOR_PIN_1, LOW);
  digitalWrite(LEFT_MOTOR_PIN_2, LOW);
}

/**
 * Motors livecycle (call in a timer)
 * @private
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
 * Ultrason livecycle (call in a timer)
 * @private
 **/
void _ultrasonLivecycle()
{
  int i = ultraCount % 10;

  ultraSum -= ultraBuffer[i];
  ultraBuffer[i] = sonar.ping_cm();
  if(ultraBuffer[i]==0) ultraBuffer[i]=100; //bugfix, when nothing detected the lib return 0
  ultraSum += ultraBuffer[i];
  if (debugUltrason) Serial.println(ultraSum / 10);

  hasEnnemy = (ultraSum / 10 < MIN_DISTANCE_IN_CM);
  digitalWrite(ENEMY_LED_PIN, hasEnnemy);
  ultraCount++;
}

