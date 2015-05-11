#include <NewPing.h>

#define TRIGGER_PIN  20  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     21  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

/**
 * Set right motor in backward position (low level)
 * @private
 **/
void _backwardRight() {
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}


/**
 * Set right motor in forward position (low level)
 * @private
 **/
void _forwardRight() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}

/**
 * Stop right motor (freewheel)
 * @private
 **/
void _stopRight() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
}

/**
 * Set left motor in backward position (low level)
 * @private
 **/
void _backwardLeft() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
}

/**
 * Set left motor in forward position (low level)
 * @private
 **/
void _forwardLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
}

/**
 * Stop left motor (freewheel)
 * @private
 **/
void _stopLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
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
  int j = (ultraCount + 1) % 10;

  ultraBuffer[i] = sonar.ping_cm();
  ultraSum += ultraBuffer[i];
  ultraSum -= ultraBuffer[j];
  if (debugUltrason) Serial.println(ultraSum / 10);

  hasEnnemy = (ultraSum / 10 < minDistanceInCm);
  digitalWrite(enemyLedPin, hasEnnemy);
  ultraCount++;
}

