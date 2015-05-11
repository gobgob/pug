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
