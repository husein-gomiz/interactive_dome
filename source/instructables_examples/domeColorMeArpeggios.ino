// domeColorMeArpeggios - A user colors up dome with three different colors. Each color corresponds to one note of arpeggio. 1 MIDI output
// Last updated by Jon Bumstead on 7 January 2017
#include <MIDI.h>

#define numLeds 120 //24 Number of LEDs
#define numIR 24 //24 Number of IR per Triangle
#define numTri 5 // Number of rows per Triangle

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
byte red[numLeds];    // Initialize array with vallues for LED strip
byte green[numLeds];    // Initialize array with vallues for LED strip
byte blue[numLeds];    // Initialize array with vallues for LED strip
int counter0[numIR];
int counter1[numIR];
int counter2[numIR];
int counter3[numIR];
int counter4[numIR];
int midArr[numTri];
int noteInd[numTri];
int irInd[numTri];

int arr0; //
int arr1; //
int arr2; //
int arr3; //
int arr4; //

// CONSTANTS //
//int ledAssignRed [4] =   {0, 102, 0,   255};
//int ledAssignGreen [4] = {0,  200,  102, 80};
//int ledAssignBlue [4] =  {0,  0,   204,  0};

//int ledAssignRed [4] =   {0, 255, 0,   0};
//int ledAssignGreen [4] = {0,  0,  255, 0};
//int ledAssignBlue [4] =  {0,  0,   0,  255};

//int ledAssignRed [4] =   {0, 200, 200,   0};
//int ledAssignGreen [4] = {0, 200,  80,  60};
//int ledAssignBlue [4] =  {0, 200,   0,  250};


//int ledAssignRed [4] =   {200, 220, 200,   0};
//int ledAssignGreen [4] = {200, 0,  80,  255};
//int ledAssignBlue [4] =  {200, 0,   0,  40};

//int ledAssignRed [4] =   {200, 220, 200,   0};
//int ledAssignGreen [4] = {200, 0,  80,  255};
//int ledAssignBlue [4] =  {200, 0,   0,  40};

//int ledAssignRed [4] =   {0, 200, 200,   120};
//int ledAssignGreen [4] = {0,  0,  40, 220};
//int ledAssignBlue [4] =  {0,  255,   0,  80};

int ledAssignRed [4] =   {200, 255, 0,   0};
int ledAssignGreen [4] = {200,  0,  255, 0};
int ledAssignBlue [4] =  {200,  0,   0,  255};

int notes[24][4] = {{60, 64, 67, 72},  {61, 65, 68, 73},  {62, 66, 69, 74},  {63, 67, 70, 75},  {64, 68, 71, 76},  {65, 69, 72, 77},  {66, 70, 73, 78},   {67, 71, 74, 79},          {68, 72, 75, 80},  {69, 73, 76, 81},  {70, 74, 77, 82},  {71, 75, 78, 83},  {72, 76, 79, 84},  {73, 77, 80, 85},  {74, 78, 81, 86}, {75, 79, 82, 87},          {76, 80, 83, 88},  {77, 81, 84, 89},  {78, 82, 85, 90},  {79, 83, 86, 91},  {80, 84, 87, 92},  {81, 85, 88, 93},  {82, 86, 89, 94},   {83, 87, 90, 95}};
int pauseMidi = 4000;
int delayTimeTot = 140;

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  // put your setup code here, to run once:

  DDRB = B11111111; // sets Arduino pins 8 to 13 as inputs
  //DDRD = DDRD | B11111100; // sets Arduino pins 2 to 7 as output without changing the value of pins 0 & 1, which are RX & TX

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

  memset(red, 0, numLeds);
  memset(green, 0, numLeds);
  memset(blue, 0, numLeds);
  memset(counter0, 0, numIR);
  memset(counter1, 0, numIR);
  memset(counter2, 0, numIR);
  memset(counter3, 0, numIR);
  memset(counter4, 0, numIR);

  updatestring(dataPin);

}

void loop() {
  // put your main code here, to run repeatedly:
  PORTB = B00000000; // SET control pins for mux low

  for (int i = 0; i < numIR; i++) {


    arr0 = digitalRead(m0);
    arr1 = digitalRead(m1);
    arr2 = digitalRead(m2);
    arr3 = digitalRead(m3);
    arr4 = digitalRead(m4);


    // TRIANGLE 0 /////////////////////////////////////////////////////
    if (arr0 == 0)
    {
      arr0 = 1;
      midArr[0] = 1;
      noteInd[0] = counter0[i];
      irInd[0] = i;
    }
    else
    {
      arr0 = 0;
    }

    red[i] = ledAssignRed[arr0 + counter0[i]];
    green[i] = ledAssignGreen[arr0 + counter0[i]];
    blue[i] = ledAssignBlue[arr0 + counter0[i]];

    counter0[i] = counter0[i] + arr0;

    if (counter0[i] == 4)
    {
      counter0[i] = 0;
    }
    ///////////////////////////////////////////////////////////////////

    // TRIANGLE 1 /////////////////////////////////////////////////////
    if (arr1 == 0)
    {
      arr1 = 1;
      midArr[1] = 1;
      noteInd[1] = counter1[i]; // cycles 1 - 4 to select which part of arpeggio to play
      irInd[1] = i;  // base note of the arpeggio
    }
    else
    {
      arr1 = 0;
    }

    red[i + 24] = ledAssignRed[arr1 + counter1[i]];
    green[i + 24] = ledAssignGreen[arr1 + counter1[i]];
    blue[i + 24] = ledAssignBlue[arr1 + counter1[i]];

    counter1[i] = counter1[i] + arr1;

    if (counter1[i] == 4)
    {
      counter1[i] = 0;
    }
    ///////////////////////////////////////////////////////////////////


    // TRIANGLE 2 /////////////////////////////////////////////////////
    if (arr2 == 0)
    {
      arr2 = 1;
      midArr[2] = 1;
      noteInd[2] = counter2[i];
      irInd[2] = i;
    }
    else
    {
      arr2 = 0;
    }

    red[i + 48] = ledAssignRed[arr2 + counter2[i]];
    green[i + 48] = ledAssignGreen[arr2 + counter2[i]];
    blue[i + 48] = ledAssignBlue[arr2 + counter2[i]];

    counter2[i] = counter2[i] + arr2;

    if (counter2[i] == 4)
    {
      counter2[i] = 0;
    }
    //////////////////////////////////////////////////////////////////



    // TRIANGLE 3 /////////////////////////////////////////////////////
    if (arr3 == 0)
    {
      arr3 = 1;
      midArr[3] = 1;
      noteInd[3] = counter3[i];
      irInd[3] = i;
    }
    else
    {
      arr3 = 0;
    }

    red[i + 72] = ledAssignRed[arr3 + counter3[i]];
    green[i + 72] = ledAssignGreen[arr3 + counter3[i]];
    blue[i + 72] = ledAssignBlue[arr3 + counter3[i]];

    counter3[i] = counter3[i] + arr3;

    if (counter3[i] == 4)
    {
      counter3[i] = 0;
    }
    /////////////////////////////////////////////////////////////////


    // TRIANGLE 4 /////////////////////////////////////////////////////
    if (arr4 == 0)
    {
      arr4 = 1;
      midArr[4] = 1;
      noteInd[4] = counter4[i];
      irInd[4] = i;
    }
    else
    {
      arr4 = 0;
    }

    red[i + 96] = ledAssignRed[arr4 + counter4[i]];
    green[i + 96] = ledAssignGreen[arr4 + counter4[i]];
    blue[i + 96] = ledAssignBlue[arr4 + counter4[i]];

    counter4[i] = counter4[i] + arr4;

    if (counter4[i] == 4)
    {
      counter4[i] = 0;
    }
    /////////////////////////////////////////////////////////////////


    PORTB ++; // increment MUX

  }
  updatestring(dataPin);
  delay(delayTimeTot);


}


////////////////////////////////////
void updatestring(int ledS)
{
  for (int i = 0; i < numLeds; i++)
  {
    shiftOut(ledS, clockPin, MSBFIRST, red[i]);
    shiftOut(ledS, clockPin, MSBFIRST, green[i]);
    shiftOut(ledS, clockPin, MSBFIRST, blue[i]);
  }

  PORTB = B00000000; // SET control pins for mux low
  if (midArr[0] == 1)
  {
    MIDI.sendNoteOn(notes[irInd[0]][noteInd[0]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[0]][noteInd[0]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  // PORTB ++; // increment MUX
  if (midArr[1] == 1)
  {
    MIDI.sendNoteOn(notes[irInd[1]][noteInd[1]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[1]][noteInd[1]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  // PORTB ++; // increment MUX
  if (midArr[2] == 1)
  {
    MIDI.sendNoteOn(notes[irInd[2]][noteInd[2]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[2]][noteInd[2]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  // PORTB ++; // increment MUX
  if (midArr[3] == 1)
  {
    MIDI.sendNoteOn(notes[irInd[3]][noteInd[3]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[3]][noteInd[3]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  // PORTB ++; // increment MUX
  if (midArr[4] == 1)
  {
    MIDI.sendNoteOn(notes[irInd[4]][noteInd[4]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[4]][noteInd[4]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  //midArr={0,0,0,0,0};
  midArr[0] = 0;
  midArr[1] = 0;
  midArr[2] = 0;
  midArr[3] = 0;
  midArr[4] = 0;

}
//////////////////////////////////
