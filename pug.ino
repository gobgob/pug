#include <Servo.h>

#define rightMotorPin1 12
#define rightMotorPin2 11

#define leftMotorPin1 9
#define leftMotorPin2 10

#define starterServoPin 18

#define rotateTime 750

// Globals vars
Servo starterServo;

// # Low level controls
// ## Motor Right
void backwardRight() {
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

void forwardRight() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}

void stopRight() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
}

// ## Motor Left
void backwardLeft() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
}

void forwardLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
}

void stopLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
}

volatile int loopCounter = 0;
volatile int leftSpeed = 100;
volatile int rightSpeed = 100;

// Motors controls methods
void setLeftSpeed(int speed) {
  leftSpeed = speed + 100;
}

void setRightSpeed(int speed) {
  rightSpeed = speed + 100;
}

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
  delay(rotateTime);
  stopMotors();
}

/**
 * Rotate of +/- 90° to left
 **/
void rotateLeft() {
  setLeftSpeed( -60 );
  setRightSpeed( 60 );
  delay(rotateTime);
  stopMotors();
}

// Servo control methods
void smoothSploch () {
  for (int i = 135; i > 30; i--) {
    starterServo.write(i);
    delay(50);
  }
  starterServo.detach();
  initializeTimer1();
}

void prepareToStart () {
  starterServo.attach(starterServoPin);
  starterServo.write(135);
}

/**
 * Interrupt service routine
 **/
ISR(TIMER1_OVF_vect)
{
  loopCounter = (loopCounter + 1) % 200;
  TCNT1 = 64286; // preload timer

  if (leftSpeed == 100) stopLeft();
  else if (loopCounter >= leftSpeed)forwardLeft();
  else backwardLeft();

  if (rightSpeed == 100) stopRight();
  else if (loopCounter >= rightSpeed)forwardRight();
  else backwardRight();
}

void initializeTimer1 () {
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 64286;            // preload timer
  TCCR1B |= (1 << CS10);    // 0 prescaler
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

void setup() {
  // Motor Right
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Motor Left
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  // Serial
  Serial.begin(9600);

  initializeTimer1();
}

void loop() {
  // TODO Script to win here ;)
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
    case 'I': // Initialize
      setLeftSpeed(20);
      break;
    case 'R': // Rotation
      if (serialInput[1] == 'L') rotateLeft();
      else rotateRight();
      break;

    case 'F': // Front direction
      setLeftSpeed( serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]);
      setRightSpeed( serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]);
      delay(1500);
      setLeftSpeed( -2 );
      setRightSpeed( -2);
      delay(400);
      stopMotors();
      break;

    case 'B': // Back direction
      setLeftSpeed( (serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]) * -1);
      setRightSpeed( (serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]) * -1);

      break;

    case 'S': // Starting block
      prepareToStart();
      break;

    case 'G': // GO !!!
      smoothSploch();
      break;

    case 'w': // Emergy stop
      stopMotors();
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
