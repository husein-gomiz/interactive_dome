#include <MIDI.h>

#define numLeds 120 //24 Number of LEDs
#define numIR 24 //24 Number of IR per Triangle
#define numTri 5 // Number of rows per Triangle
#define numElements 20 // number of elements in sequence

// OUTPUTS //
int clockPin = A1;
int dataPin = A0;
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
int red[numLeds];    // Initialize array with vallues for LED strip
int green[numLeds];    // Initialize array with vallues for LED strip
int blue[numLeds];    // Initialize array with vallues for LED strip

int arr0r; // reading from MUX
int arr1r; // reading from MUX
int arr2r; // reading from MUX
int arr3r; // reading from MUX
int checkOn;
int z;
int guess;
int p = 1;
int midA;
int note2play;
int lost = 0;
int score;
int highScore = 0;
int input;

// CONSTANTS //
int notes[24][4] = {{60, 64, 67, 72},  {61, 65, 68, 73},  {62, 66, 69, 74},  {63, 67, 70, 75},  {64, 68, 71, 76},  {65, 69, 72, 77},  {66, 70, 73, 78},   {67, 71, 74, 79},          {68, 72, 75, 80},  {69, 73, 76, 81},  {70, 74, 77, 82},  {71, 75, 78, 83},  {72, 76, 79, 84},  {73, 77, 80, 85},  {74, 78, 81, 86}, {75, 79, 82, 87},          {76, 80, 83, 88},  {77, 81, 84, 89},  {78, 82, 85, 90},  {79, 83, 86, 91},  {80, 84, 87, 92},  {81, 85, 88, 93},  {82, 86, 89, 94},   {83, 87, 90, 95}};
int note = 60;
int pauseMidi = 4000;
int pauseMidi2 = 50;
int loseDelay = 80;
int elementDelayHi = 500; // delay
int elementDelayLo = 10; // delay
int pattern[numElements];
int rHi = 255;
int gHi = 0;
int bHi = 0;
int rLo = 3;
int gLo = 3;
int bLo = 3;
int rLoS = 0;
int gLoS = 0;
int bLoS = 0;

int rHiG = 0;
int gHiG = 255;
int bHiG = 0;

int rLose = 255;
int gLose = 0;
int bLose = 0;

int rC = 200;
int gC = 0;
int bC = 255;

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

  for (int i = 0; i < numElements; i++)
  {
    pattern[i] = random(0, numLeds - numIR-1); // create a pattern of numElements with numbers ranging from 0 to numLeds-1
  }

  for (int i = 0; i < numLeds - numIR; i++)
  {
    red[i] = rLo;
    green[i] = gLo;
    blue[i] = bLo;
  }

  for (int i = numLeds - numIR; i < numLeds; i++)
  {
    red[i] = rLoS;
    green[i] = gLoS;
    blue[i] = bLoS;
  }

  updatestring();

}

void loop() {

  delay(100);
  for (int d = 0; d < p; d++) // color dome to display pattern for user
  {

    red[pattern[d]] = rHi;
    green[pattern[d]] = gHi;
    blue[pattern[d]] = bHi;

    midA = 1;
    note2play = d;

    updatestring();
    delay(elementDelayHi);

    red[pattern[d]] = rLo;
    green[pattern[d]] = gLo;
    blue[pattern[d]] = bLo;

    updatestring();
    delay(elementDelayLo);

    score = d;

  }


  checkOn = 0;
  z = 0;
  while (checkOn == 0)
  {


    PORTB = B00000000; // SET control pins for mux low
    for (int i = 0; i < numIR; i++) {

      arr0r = digitalRead(m0);
      arr1r = digitalRead(m1);
      arr2r = digitalRead(m2);
      arr3r = digitalRead(m3);
      //arr4r = digitalRead(m4);

      PORTB ++; // increment MUX

      if (arr0r == 0 || arr1r == 0 || arr2r == 0 || arr3r == 0) // a button has been clicked! //
      {
        if (arr0r == 0)
        {
          input = i;
        }
        else if (arr1r == 0)
        {
          input = i + numIR;
        }
        else if (arr2r == 0)
        {
          input = i + numIR * 2;
        }
        else if (arr3r == 0)
        {
          input = i + numIR * 3;
        }

        guess = pattern[z] - input; // if this value is zero you are correct!

        red[input] = rHiG; // assign leds to light up for guess
        green[input] = gHiG; // assign leds to light up for guess
        blue[input] = bHiG; // assign leds to light up for guess

        midA = 1;
        note2play = z + 12; //i;
        updatestring();

        delay(elementDelayHi / 2);

        red[input] = rLo;
        green[input] = gLo;
        blue[input] = bLo;

        updatestring();
        delay(elementDelayLo / 2);

        if (guess == 0) // YOU ARE CORRECT
        {
          z++;
          if (z == p)
          {
            checkOn = 1;
          }
        }
        else // YOU LOST
        {
          updatestringLose();
          delay(700);

          updatestringCount();
          delay(1200);
          checkOn = 1;
          lost = 1;
        }


      }
    }
  }

  if (p < numElements && lost == 0)
  {
    p++;


  }
  else if (p >= numElements || lost == 1)
  {
    p = 1;
    if (lost == 1)
    {
      lost = 0;
    }
  }

}

////////////////////////////////////
void updatestring()
{
  for (int i = 0; i < numLeds; i++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
  }

  PORTB = B00000000; // SET control pins for mux low

  if (midA == 1)
  {
    MIDI.sendNoteOn(notes[note2play][0], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[note2play][0], 127, 1);
    delay(pauseMidi2);

    MIDI.sendNoteOn(notes[note2play][1], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[note2play][1], 127, 1);
    delay(pauseMidi2);

    MIDI.sendNoteOn(notes[note2play][2], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[note2play][2], 127, 1);
    delay(pauseMidi2);

    MIDI.sendNoteOn(notes[note2play][3], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[note2play][3], 127, 1);
    delay(pauseMidi2);
  }

  midA = 0;

}
//////////////////////////////////

////////////////////////////////////
void updatestringLose()
{
  delay(500);
  for (int i = 0; i < numLeds; i++)
  {
    red[i] = rLo;
    green[i] = gLo;
    blue[i] = bLo;
  }

  for (int i = 0; i < numLeds; i++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, 240);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
  }
  delay(200);

  for (int i = 0; i < numLeds; i++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, rLo);
    shiftOut(dataPin, clockPin, MSBFIRST, gLo);
    shiftOut(dataPin, clockPin, MSBFIRST, bLo);
  }
  delay(200);

  MIDI.sendNoteOn(70, 127, 1);
  delayMicroseconds(pauseMidi);
  MIDI.sendNoteOff(70, 127, 1);
  delayMicroseconds(pauseMidi);

  delay(100);


  for (int m = 0; m < 2; m++)
  {

    for (int i = 0; i < numLeds; i++)
    {
      shiftOut(dataPin, clockPin, MSBFIRST, 240);
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
    }
    delay(60);

    for (int i = 0; i < numLeds; i++)
    {
      shiftOut(dataPin, clockPin, MSBFIRST, rLo);
      shiftOut(dataPin, clockPin, MSBFIRST, gLo);
      shiftOut(dataPin, clockPin, MSBFIRST, bLo);
    }
    delay(60);

    MIDI.sendNoteOn(74, 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(74, 127, 1);
    delayMicroseconds(pauseMidi);
  }

  for (int m = 0; m < 4; m++)
  {
    delay(80 - m * 5);

    MIDI.sendNoteOn(75 - m * 2, 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(75 - m * 2, 127, 1);
    delayMicroseconds(pauseMidi);
  }

}
//////////////////////////////////


////////////////////////////////////
void updatestringCount()
{

  PORTB = B00000000; // SET control pins for mux low

  for (int m = 0; m < score; m++)
  {

    red[m + numIR * 4] = rC;
    green[m + numIR * 4] = gC;
    blue[m + numIR * 4] = bC;

    for (int i = 0; i < numLeds; i++)
    {
      shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
      shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
      shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
    }

    delay(loseDelay);

    MIDI.sendNoteOn(notes[m][0], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[m][0], 127, 1);
    delay(pauseMidi2);

    MIDI.sendNoteOn(notes[m][1], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[m][1], 127, 1);
    delay(pauseMidi2);

    MIDI.sendNoteOn(notes[m][2], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[m][2], 127, 1);
    delay(pauseMidi2);

    MIDI.sendNoteOn(notes[m][3], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[m][3], 127, 1);
    delay(pauseMidi2);
  }

  for (int m = 0; m < 6; m++) // BLINKING YOUR CURRENT SCORE
  {

    for (int i = 0; i < numLeds - numIR; i++)
    {
      shiftOut(dataPin, clockPin, MSBFIRST, rLo);
      shiftOut(dataPin, clockPin, MSBFIRST, gLo);
      shiftOut(dataPin, clockPin, MSBFIRST, bLo);
    }
  
    for (int i = numLeds - numIR; i < numLeds; i++)
    {
      shiftOut(dataPin, clockPin, MSBFIRST, rLoS);
      shiftOut(dataPin, clockPin, MSBFIRST, gLoS);
      shiftOut(dataPin, clockPin, MSBFIRST, bLoS);
    }
    
    delay(loseDelay);

    for (int i = 0; i < numLeds; i++)
    {
      shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
      shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
      shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
    }
    delay(loseDelay);

  }

  // RESET BASE COLOR OF DOME
  for (int i = 0; i < numLeds - numIR; i++)
  {
    red[i] = rLo;
    green[i] = gLo;
    blue[i] = bLo;
  }

  for (int i = numLeds - numIR; i < numLeds; i++)
  {
    red[i] = rLoS;
    green[i] = gLoS;
    blue[i] = bLoS;
  }


  if (score > highScore) // DISPLAYING HIGH SCORE
  {
    highScore = score;
  }


  for (int m = 0; m < highScore; m++)
  {

    red[m + numIR * 4] = rC;
    green[m + numIR * 4] = gC;
    blue[m + numIR * 4] = bC;
  }

  for (int i = 0; i < numLeds; i++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
    shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
  }


  // New Pattern

  for (int i = 0; i < numElements; i++)
  {
    pattern[i] = random(0, numLeds - numIR-1);
  }



  //  // reset dome:
  //  for (int m = 0; m < score; m++)
  //  {
  //
  //    red[m] = rLo;
  //    blue[m] = bLo;
  //    green[m] = gLo;
  //
  //  }
  //
  //  for (int i = 0; i < numLeds; i++)
  //  {
  //    shiftOut(dataPin, clockPin, MSBFIRST, red[i]);
  //    shiftOut(dataPin, clockPin, MSBFIRST, blue[i]);
  //    shiftOut(dataPin, clockPin, MSBFIRST, green[i]);
  //  }

}
//////////////////////////////////

