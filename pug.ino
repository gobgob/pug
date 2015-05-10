#include <Servo.h>

#define rightMotorPin1 11
#define rightMotorPin2 10

#define leftMotorPin1 8
#define leftMotorPin2 9

IntervalTimer motorTimer;

#define ultraTrig 20
#define ultraEcho 21

#define starterServoPin 19

#define rotateTimeLeft 570
#define rotateTimeRight 500

#define pinColor 2

#define JUMPER 3
bool endScript = false;
IntervalTimer ultraTimer;
int ultraBuffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ultraCompteur;
int ultraSum;

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

long ultraMesure() {
  /* Utilisation du capteur Ultrason HC-SR04 */

  long lecture_echo;
  long cm;

  digitalWrite(ultraTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraTrig, LOW);
  lecture_echo = pulseIn(ultraEcho, HIGH, 500000);
  cm = lecture_echo / 29;

  return cm;
}

void ultraCheck(void) {
  int i = ultraCompteur % 10;
  int first_i = (ultraCompteur + 1) % 10;
  ultraBuffer[i] = ultraMesure();
  ultraSum += ultraBuffer[i];
  ultraSum -= ultraBuffer[first_i];
  Serial.println(ultraSum / 10);

  if (ultraSum / 10 < 20) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  ultraCompteur++;
}

boolean isGreen(){
 return digitalRead(pinColor);  
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

// Servo control methods
void smoothSploch () {
  for (int i = 135; i > 30; i--) {
    starterServo.write(i);
    delay(50);
  }
  starterServo.detach();
  //  initializeTimer1();
}

void prepareToStart () {
  starterServo.attach(starterServoPin);
  starterServo.write(135);
}

/**
 * Interrupt service routine
 **/
void manageMotors()
{
  loopCounter = (loopCounter + 1) % 200;

  if (rightSpeed == 100) stopRight();
  else if (loopCounter >= rightSpeed)forwardRight();
  else backwardRight();

  if (leftSpeed == 100) stopLeft();
  else if (loopCounter >= leftSpeed)forwardLeft();
  else backwardLeft();

}

void setup() {
  // Motor Right
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // Motor Left
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  motorTimer.begin(manageMotors, 30);

  // Ultrason
 // ultraTimer.begin(ultraCheck, 50000);
  pinMode(ultraTrig, OUTPUT);
  digitalWrite(ultraTrig, LOW);
  pinMode(ultraEcho, INPUT);
  ultraCompteur = 1;
  ultraSum = 0;

  // PinColor
  pinMode(pinColor, INPUT);

  //DebugLed
  pinMode(13, OUTPUT);
  
  //Init Jumper
  pinMode(JUMPER, INPUT);

  // Serial
  Serial.begin(9600);
  
    if (isGreen()) {
    digitalWrite(13,LOW);
  } else {
    digitalWrite(13,HIGH);
  }
  

  //  initializeTimer1();
}

void loop() {
  
  if (!endScript){
    for (int i = 0; i<2000; i++);
    Serial.println("DEBUT DE SCRIPT");
    Serial.println("Attente de Jumper...");
    while(!digitalRead(JUMPER));
    Serial.println("Debout!");
    prepareToStart();
    Serial.println("Retirer le jumper...");
    while(digitalRead(JUMPER));
    
    
    endScript = true;
    Serial.println("FIN DE SCRIPT");
    while(42);
    }
  
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
      setRightSpeed( 30);
      break;

    case 'l':
    case 'L':
      Serial.println("left on");
      setLeftSpeed( 30);
      break;
      
    case 'j':
    case 'J':
      JUMPER != JUMPER;
      if (JUMPER) Serial.println("Jumper On!");
      else Serial.println("Jumper off");
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
