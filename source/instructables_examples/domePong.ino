// domePong
// Last updated by Jon Bumstead on 7 January 2017

#include <MIDI.h> // include MIDI library


#define numLeds 120 // Number of LEDs
#define numIR 24 // Number of IR per big triangle
#define numIRp 8 // Number of IR per big triangle on base
#define numTri 5 // Number of big triangle sections
#define numBase 40 // Number of base LEDs

// OUTPUTS //
int clockPin = A1; // clock pin for LED strip
int dataPin = A0; // data pin for LED strip
int s0 = 8; // MUX control 0 - PORTbD
int s1 = 9; // MUX control 1 - PORTb
int s2 = 10; // MUX control 2 - PORTb
int s3 = 11; // MUX control 3 - PORTb
int s4 = 12; // MUX control 4 - PORTb

// INPUTS //
int m0 = 3;  // MUX input 0
int m1 = 4;  // MUX input 0
int m2 = 5;  // MUX input 0
int m3 = 6;  // MUX input 0
int m4 = 7;  // MUX input 0

// VARIABLES //
byte red[numLeds];     // Initialize array with vallues for LED strip
byte green[numLeds];   // Initialize array with vallues for LED strip
byte blue[numLeds];    // Initialize array with vallues for LED strip
bool arr[numBase];  // digital read from MUX0
bool arrReshape[numBase];
int indReshapeA[numBase];
int currentPosition = 5; // position of pad, set at start position 5
int nextPosition;
int LEDind;
int offCenter;
int startRunOff;

int nextLEDind;
int padDist[numBase];
int maxDist;
int padPos;
int padDir;
byte numMoves;

byte nextRed;
byte nextGreen;
byte nextBlue;

// CONSTANTS //
int pauseMidi = 4000;
byte rLo = 0;
byte gLo = 0;
byte bLo = 1;
byte LEDbaseInd[numBase];
byte base2IRtri[numBase][2]; // map from current position to IR and triangle
byte LEDbaseCenter = 20;
byte rPad1 = 220;
byte gPad1 = 100;
byte bPad1 = 100;
byte nextr1 = 40;
byte nextg1 = 3;
byte nextb1 = 0;
byte timePerBallMove = 210;
byte numMovesPerCycle = 7;
byte timePerPadMove = timePerBallMove / numMovesPerCycle; // amount of time for pad to move one space

// BALL CONSTANTS //
byte currentBallPos[] = {16, 0};
byte ballIndArray[] = {0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4};
byte indBall[] = {5, 6, 7, 8, 9};
byte currentBallTri = 0;
byte LED2IR[numIR][numTri];
byte posTraj = 0;
byte startBallTri = 0;
byte startPath = 0;
byte ballRed = 255;
byte ballGreen = 255;
byte ballBlue = 255;
byte indBallLED[8];

int safeSpot[8][3][2] = { {{14, 0}, {15, 0}, {9, -1}}  ,
  {{13, 0}, {14, 0}, {15, 0}}  ,
  {{12, 0}, {13, 0}, {14, 0}}  ,
  {{11, 0}, {12, 0}, {13, 0}}  ,
  {{10, 0}, {11, 0}, {12, 0}}  ,
  {{9, 0}, {10, 0}, {11, 0}}   ,
  {{9, 0}, {10, 0}, {15, 1}}   ,
  {{15, 1}, {9, 0}, {15, 1}}
};


MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  // put your setup code here, to run once:

  DDRB = B11111111; // sets Arduino pins 8 to 13 as inputs
  //DDRD = DDRD | B11111100; // sets Arduino pins 2 to 7 as output without changing the value of pins 0 & 1, which are RX & TX
  //DDRD = DDRD | B00000000; // sets Arduino pins 2 to 7 as input without changing the value of pins 0 & 1, which are RX & TX

  MIDI.begin(MIDI_CHANNEL_OFF);

  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(s4, OUTPUT);

  pinMode(m0, INPUT);
  pinMode(m1, INPUT);
  pinMode(m2, INPUT);
  pinMode(m3, INPUT);
  pinMode(m4, INPUT);

  for (int j = 0; j < 5; j++)
  {
    for (int i = 0; i < numIRp; i++)
    {
      LEDbaseInd[i + j * numIRp] = i + 16 + numIR * j;
      base2IRtri[i + j * numIRp][0] = i + 16;
      base2IRtri[i + j * numIRp][1] = j;
    }
  }

  for (int j = 0; j < 5; j++)
  {
    for (int i = 0; i < numIR; i++)
    {
      LED2IR[i][j] = i + numIR * j;
    }
  }

  for (int i = 0; i < numLeds; i++)
  {
    red[i] = rLo;
    green[i] = gLo;
    blue[i] = bLo;
  }

  LEDind = LEDbaseInd[currentPosition];
  for (int j = -1; j < 2; j++) //
  {
    red[j + LEDind] = rPad1;
    green[j + LEDind] = gPad1;
    blue[j + LEDind] = bPad1;
  }

  for (int i = 0; i < numBase; i++)
  {
    padDist[i] = i - LEDbaseCenter;
  }

  for (int i = 0; i < numLeds; i++) // Shift output for LED strip
  {
    shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
  }

  //Serial.begin(9600);
}

void loop()
{

  PORTB = B00010000; // SET control pins for MUX to start at IR sensor 16
  //Serial.println(" ");

  for (int i = 0; i < numIRp; i++) {

    // Digital read output of MUX0 - MUX4 for IR sensor i
    // If IR sensor is LO, the triangle is being touched by player.
    arr[i] = digitalRead(m0);
    arr[i + numIRp] = digitalRead(m1);
    arr[i + numIRp * 2] = digitalRead(m2);
    arr[i + numIRp * 3] = digitalRead(m3);
    arr[i + numIRp * 4] = digitalRead(m4);
    PORTB++;

  }

  maxDist = 0;//0;
  arr[currentPosition] = 0; // set current position low so that if no other triangle is touched, pad will remain in current position
  offCenter = LEDbaseCenter - currentPosition;
  getArrReshape(offCenter, arr, arrReshape, indReshapeA);

  //Serial.println("START");
  for (int i = 0; i < numBase; i++)
  {

    int currentDist = abs(maxDist);
    int testDist = abs(padDist[i]);

    if ((arrReshape[i] == 0) && (testDist > currentDist)) // searching for triangle that was pressed farthest from current pad position
    {
      maxDist = padDist[i]; // new maximum distance
      nextPosition = indReshapeA[i];
    }
  }

  numMoves = abs(maxDist);
  if (numMoves > numMovesPerCycle)
  {
    numMoves = numMovesPerCycle;
  }

  if (maxDist > 0)
  {
    padDir = 1;
    nextRed = nextr1;
    nextGreen = nextg1;
    nextBlue = nextb1;
  }
  else if (maxDist < 0)
  {
    padDir = -1;
    nextRed = nextr1;
    nextGreen = nextg1;
    nextBlue = nextb1;
  }
  else if (maxDist == 0)
  {
    padDir = 0;
    nextRed = rPad1;
    nextGreen = gPad1;
    nextBlue = bPad1;
  }
  //Serial.println(numMoves);
  updatestring();

}


////////////////////////////////////
void updatestring()
{
  //int path0[9][2] = {{15, 0}, {8, -1}, {7, -1}, {3, -1}, {1, -2}, {5, -2}, {4, -2}, {9, -3}, {22, -3}};



  for (int d = 0; d < numMoves; d++)
  {

    for (int j = -1; j < 2; j++) // erase current position of pad to default color
    {

      if (j + currentPosition == numBase)
      {
        LEDind = LEDbaseInd[0];
      }
      else if (j + currentPosition == numBase + 1)
      {
        LEDind = LEDbaseInd[1];
      }
      else if (j + currentPosition == -1)
      {
        LEDind = LEDbaseInd[numBase - 1];
      }
      else if (j + currentPosition == -2)
      {
        LEDind = LEDbaseInd[numBase - 2];
      }
      else
      {
        LEDind = LEDbaseInd[j + currentPosition];
      }

      red[LEDind] = rLo;
      green[LEDind] = gLo;
      blue[LEDind] = bLo;
      // Serial.println(LEDind);
    }

    currentPosition = currentPosition + padDir; // update position

    if (currentPosition == numBase)
    {
      currentPosition = 0;
    }
    else if (currentPosition == -1)
    {
      currentPosition = numBase - 1;
    }


    red[LEDbaseInd[nextPosition]] = nextRed;
    green[LEDbaseInd[nextPosition]] = nextGreen;
    blue[LEDbaseInd[nextPosition]] = nextBlue;

    for (int j = -1; j < 2; j++) // color pad in new position
    {
      if (j + currentPosition == numBase)
      {
        LEDind = LEDbaseInd[0];
      }
      else if (j + currentPosition == -1)
      {
        LEDind = LEDbaseInd[numBase - 1];
      }
      else //if ((j + currentPosition > 0) && (j + currentPosition < numBase))
      {
        LEDind = LEDbaseInd[j + currentPosition];
      }

      red[LEDind] = rPad1;
      green[LEDind] = gPad1;
      blue[LEDind] = bPad1;

    }

    for (int i = 0; i < numLeds; i++) // Shift output for LED strip
    {
      shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
      shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
      shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
    }

    delay(timePerPadMove * 0.5);

    PORTB = B00000000; // SET control pins for MUX to start at IR sensor 16
    MIDI.sendNoteOn(60, 127, 1); // make sound for moving pad
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(60, 127, 1);
    delayMicroseconds(pauseMidi);

  }

  int path0[9][2];
  if (startPath == 0) // IR 16
  {
    int path0[9][2] = {{15, 0}, {8, -1}, {7, -1}, {3, -1}, {1, -2}, {5, -2}, {4, -2}, {9, -3}, {22, -3}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }
  else if (startPath == 1) // IR 17
  {
    int path0[9][2] = {{14, 0}, {4, 0}, {1, -1}, {2, -1}, {2, -2}, {3, -2}, {8, -3}, {10, -3}, {21, -3}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }
  else if (startPath == 2) // IR 18
  {
    int path0[9][2] = {{13, 0}, {5, 0}, {3, 0}, {0, -1}, {0, -2}, {1, -3}, {7, -3}, {11, -3}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }
  else if (startPath == 3) // IR 19
  {
    int indPathA[] = { -2, 2};
    byte indPath3 = random(0, 1);
    int path0[9][2] = {{12, 0}, {6, 0}, {2, 0}, {0, 0}, {0, indPathA[indPath3]}, {2, indPathA[indPath3]}, {6, indPathA[indPath3]}, {12, indPathA[indPath3]}, {19, indPathA[indPath3]}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }
  else if (startPath == 4) // IR 20
  {
    int path0[9][2] = {{11, 0}, {7, 0}, {1, 0}, {0, 1}, {0, 2}, {3, 3}, {5, 3}, {13, 3}, {18, 3}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }
  else if (startPath == 5) // IR 21
  {
    int path0[9][2] = {{10, 0}, {8, 0}, {3, 1}, {2, 1}, {2, 2}, {1, 2}, {4, 3}, {14, 3}, {17, 3}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }

  else if (startPath == 6) // IR 22
  {
    int path0[9][2] = {{9, 0}, {4, 1}, {5, 1}, {1, 1}, {3, 2}, {7, 2}, {8, 2}, {15, 3}, {16, 3}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }

  else if (startPath == 7) // IR 23
  {
    int path0[9][2] = {{15, 1}, {14, 1}, {6, 1}, {7, 1}, {5, 2}, {6, 2}, {10, 2}, {9, 2}, {23, 2}};
    currentBallTri = ballIndArray[indBall[startBallTri] + path0[posTraj][1]];
    currentBallPos[0] = path0[posTraj][0];
  }


  currentBallPos[1] = currentBallTri;
  indBallLED[posTraj] = LED2IR[currentBallPos[0]][currentBallPos[1]];

  red[indBallLED[posTraj]] = ballRed;
  green[indBallLED[posTraj]] = ballGreen;
  blue[indBallLED[posTraj]] = ballBlue;

  if (posTraj > 0)
  {

    red[indBallLED[posTraj - 1]] = rLo;
    green[indBallLED[posTraj - 1]] = gLo;
    blue[indBallLED[posTraj - 1]] = bLo;

  }

  if (posTraj < 7)
  {

    if (posTraj == 0)
    {
      red[indBallLED[7]] = rLo;
      green[indBallLED[7]] = gLo;
      blue[indBallLED[7]] = bLo;
    }

    posTraj = posTraj + 1;

  }
  else
  {

    posTraj = 0;
    getTraj(currentPosition, currentBallPos);

  }

  for (int i = 0; i < numLeds; i++) // Shift output for LED strip
  {
    shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
  }

  delay((numMovesPerCycle - numMoves)*timePerPadMove);

}
////////////////////////////////

byte getTraj(int currentPositionA, byte currentBallPositionA[2])
{

  byte currentPadIR = base2IRtri[currentPositionA][0]; // current IR postion of pad
  byte currentPadTri = base2IRtri[currentPositionA][1]; // current Triangle postion of pad
  byte nextStartPathInd[] = {6, 5, 4, 3, 2, 1, 0};

  for (int d = 0; d < 3; d++)
  {

    byte testPadTri = ballIndArray[indBall[currentPadTri] + safeSpot[currentPadIR - 16][d][1]];
    byte testPadIR = safeSpot[currentPadIR - 16][d][0];

    if ((testPadTri - currentBallPositionA[1] == 0) && (testPadIR - currentBallPositionA[0] == 0)) // ball is in safe zone
    {
      byte randNextPathInd = random(0, 2); // pick the next start path as one of the three safe spots

      startPath = nextStartPathInd[(safeSpot[currentPadIR - 16][randNextPathInd][0] - 9)]; // IR number (ranges from 15 to 9) needs to be converted to startPath (ranges from 0 to 6)
      startBallTri = ballIndArray[indBall[currentPadTri] + safeSpot[currentPadIR - 16][randNextPathInd][1]];
      
      PORTB = B00000000; // SET control pins for MUX to start at IR sensor 16
      MIDI.sendNoteOn(67, 127, 1); // make sound for moving pad
      delayMicroseconds(pauseMidi);
      MIDI.sendNoteOff(67, 127, 1);
      delayMicroseconds(pauseMidi);
      break;

    }
    else
    {
      startPath = 0;
      startBallTri = 0;
    }

  }

  return startPath;
  return startBallTri;

}

void getArrReshape(int offCenter, bool arrT[], bool arrReshape[], int indReshapeA[])
{
  if (offCenter > 0) // current position is between 0 and 19
  {
    for (int i = 0;  i < numBase - offCenter; i++)
    {

      int indReshape = i + offCenter;
      indReshapeA[indReshape] = i;
      arrReshape[indReshape] = arrT[i];
    }

    for (int i = 0; i < offCenter; i++)
    {
      indReshapeA[i] = numBase - offCenter + i;
      arrReshape[i] = arrT[numBase - offCenter + i];
    }

  }

  else if (offCenter < 0)
  {

    startRunOff = currentPosition - LEDbaseCenter;

    for (int i = 0;  i < numBase - startRunOff; i++)
    {
      indReshapeA[i] = i + startRunOff;
      arrReshape[i] = arrT[i + startRunOff];
    }

    for (int i = 0; i <= startRunOff - 1; i++)
    {
      indReshapeA[i + numBase - startRunOff] = i;
      arrReshape[i + numBase - startRunOff] = arrT[i];
    }

  }

  else if (offCenter == 0)
  {

    for (int i = 0; i < numBase; i++)
    {
      arrReshape[i] = arrT[i];
      indReshapeA[i] = i;
    }
  }

}



