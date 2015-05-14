char serialInput[100];
int serialPrompt = 0;

/**
 * Listen serial event
 **/
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

/**
 * Execute serial instruction on `\n` char
 **/
void serialExecute()
{
  switch (serialInput[0])
  {
    case 'r':
    case 'R': // Rotation
      if (serialInput[1] == 'L' || serialInput[1] == 'l') {
        rotateLeft(serialInput[2] * 100 + serialInput[3] * 10 + serialInput[4]);
        Serial.print("Step left! -> ");
        Serial.println(serialInput[2] * 100 + serialInput[3] * 10 + serialInput[4]);
      }
      else {
        rotateRight(serialInput[2] * 100 + serialInput[3] * 10 + serialInput[4]);
        Serial.print("Step right! -> ");
        Serial.println(serialInput[2] * 100 + serialInput[3] * 10 + serialInput[4]);
      }
      break;

    case 'f':
    case 'F': // Front direction
      Serial.print("Go! -> ");
      Serial.println(serialInput[1] * 1000 + serialInput[2] * 100 + serialInput[3] * 10 + serialInput[4]);
      goForward(30, serialInput[1] * 1000 + serialInput[2] * 100 + serialInput[3] * 10 + serialInput[4]);
      goBackward(1); // Break!
      delay(500);
      stopMotors();
      break;

    case 'b':
    case 'B': // Back direction
      Serial.print("Revert revert revert! -> ");
      Serial.println(serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]);
      goBackward(60);
      delay(serialInput[1] * 100 + serialInput[2] * 10 + serialInput[3]);
      goBackward(1);
      delay(100);
      stopMotors();
      break;

    case 1: // Recalage
      Serial.print("Recalage -> ");
      Serial.println(serialInput[1] * 10 + serialInput[2]);
      goBackward(serialInput[1] * 10 + serialInput[2]);
      delay(3000);
      stopMotors();
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
