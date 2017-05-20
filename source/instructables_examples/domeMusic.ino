// domeMusic - Five people play the dome simultaneously with 5 MIDI outputs. The dome is split into 5 colors for each player, and a triangle turns white when played.
// Last updated by Jon Bumstead on 7 January 2017

#include <MIDI.h> // include MIDI library

#define numLeds 120 //24 Number of LEDs
#define numIR 24 //24 Number of IR per Triangle
#define numTri 5 // Number of rows per Triangle

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
int red[numLeds];     // Initialize array with vallues for LED strip
int green[numLeds];   // Initialize array with vallues for LED strip
int blue[numLeds];    // Initialize array with vallues for LED strip
int midArr[numTri];   // Store whether or not a note has been pressed by one of the players
int irInd[numTri];    // Index of note be played by each player - five players.
// int noteInd[numTri];  // There are five players, and each player can have a different set of notes that her part of the dome plays. If all players' play the same notes, comment noteInd out.


int arr0r; // digital read from MUX0
int arr1r; // digital read from MUX1
int arr2r; // digital read from MUX2
int arr3r; // digital read from MUX3
int arr4r; // digital read from MUX4
int vH0[numIR]; // keep track of whether or not an IR sensor was hit last cycle on MUX 0
int vH1[numIR]; // keep track of whether or not an IR sensor was hit last cycle on MUX 1
int vH2[numIR]; // keep track of whether or not an IR sensor was hit last cycle on MUX 2
int vH3[numIR]; // keep track of whether or not an IR sensor was hit last cycle on MUX 3
int vH4[numIR]; // keep track of whether or not an IR sensor was hit last cycle on MUX 4

// CONSTANTS //
int ledAssignRed [5][2] =   {{255, 102}, {255, 0},  {255, 255},  {255, 200},   {255, 200}};
int ledAssignGreen [5][2] = {{255, 200}, {255, 102}, {255, 80}, {255, 0},  {255, 0}};
int ledAssignBlue [5][2] =   {{255, 0},   {255, 204},  {255, 0},   {255, 0},  {255, 204}};
int notes[numIR]={60, 61, 62, 63, 64, 65, 66, 67,    68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83};
// int notes[numIr][numTri] = {{60, 64, 67, 72},  {61, 65, 68, 73},  {62, 66, 69, 74},  {63, 67, 70, 75},  {64, 68, 71, 76},  {65, 69, 72, 77},  {66, 70, 73, 78},   {67, 71, 74, 79},          {68, 72, 75, 80},  {69, 73, 76, 81},  {70, 74, 77, 82},  {71, 75, 78, 83},  {72, 76, 79, 84},  {73, 77, 80, 85},  {74, 78, 81, 86}, {75, 79, 82, 87},          {76, 80, 83, 88},  {77, 81, 84, 89},  {78, 82, 85, 90},  {79, 83, 86, 91},  {80, 84, 87, 92},  {81, 85, 88, 93},  {82, 86, 89, 94},   {83, 87, 90, 95}};
int pauseMidi = 4000;

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

  memset(red, 255, numLeds);
  memset(green, 80, numLeds);
  memset(blue, 0, numLeds);
  memset(midArr, 0, numTri);

  for (int i = 0; i < numIR; i++)
  {
    
    vH0[i]=0;
    vH1[i]=0;
    vH2[i]=0;
    vH3[i]=0;
    vH4[i]=0;

    //    red[i]=rOff[i];
    //    green[i]=gOff[i];
    //    blue[i]=bOff[i];

  }
  
  updatestring(dataPin);


}

void loop() {
  // put your main code here, to run repeatedly:
  PORTB = B00000000; // SET control pins for mux low

  for (int i = 0; i < numIR; i++) {

    // Digital read output of MUX0 - MUX4 for IR sensor i
    // If IR sensor is LO, the triangle is being touched by player.
    arr0r = digitalRead(m0);
    arr1r = digitalRead(m1);
    arr2r = digitalRead(m2);
    arr3r = digitalRead(m3);
    arr4r = digitalRead(m4);

    // Assign color for LED strip. If read is HIGH, triangle is colored base color. If it is LO (triangle touched), then triangle is colored white
    red[i] = ledAssignRed[0][arr0r];
    green[i] = ledAssignGreen[0][arr0r];
    blue[i] = ledAssignBlue[0][arr0r];

    red[i + 24] = ledAssignRed[1][arr1r];
    green[i + 24] = ledAssignGreen[1][arr1r];
    blue[i + 24] = ledAssignBlue[1][arr1r];

    red[i + 48] = ledAssignRed[2][arr2r];
    green[i + 48] = ledAssignGreen[2][arr2r];
    blue[i + 48] = ledAssignBlue[2][arr2r];

    red[i + 72] = ledAssignRed[3][arr3r];
    green[i + 72] = ledAssignGreen[3][arr3r];
    blue[i + 72] = ledAssignBlue[3][arr3r];

    red[i + 96] = ledAssignRed[4][arr4r];
    green[i + 96] = ledAssignGreen[4][arr4r];
    blue[i + 96] = ledAssignBlue[4][arr4r];


    // TRIANGLE 0 ///////////////////////////////////
    if ((arr0r == 0) && (vH0[i]==0)) // Sensor is blocked and player 1 did not have her hand on triangle last cycle
    {
      midArr[0] = 1; // Player 0 has hit a note, set HI so that there is MIDI output for player 0
      //noteInd[0] = 0;  
      irInd[0] = i;   // Note to play for Player 0
      vH0[i]=1; // Player 0 has her hand on Triangle i. Set vH0 high so that note won't continue to play if Player 0 leaves hand on triangle i      
    }
    else if (arr0r == 1)
    {     
      vH0[i]=0; // Player 0 does not have her hand on Triangle i.
    }
    ///////////////////////////////////////////////////////



    // TRIANGLE 1 ///////////////////////////////////
    if ((arr1r == 0) && (vH1[i]==0))
    {
      midArr[1] = 1; // Player 1 has hit a note, set HI so that there is MIDI output for player 1
      //noteInd[1] = 0;  
      irInd[1] = i;   // Note to play for Player 1
      vH1[i]=1; // Player 1 has her hand on Triangle i. Set vH0 high so that note won't continue to play if Player 1 leaves hand on triangle i    
    }
    else if (arr1r == 1)
    {    
      vH1[i]=0; // Player 1 does not have her hand on Triangle i.
    }
    ///////////////////////////////////////////////////////



    // TRIANGLE 2 ///////////////////////////////////
    if ((arr2r == 0) && (vH2[i]==0))
    {
      midArr[2] = 1; // Player 2 has hit a note, set HI so that there is MIDI output for player 2
      //noteInd[2] = 0;  
      irInd[2] = i;   // Note to play for Player 2
      vH2[i]=1; // Player 2 has her hand on Triangle i. Set vH0 high so that note won't continue to play if Player 2 leaves hand on triangle i    
    }
    else if (arr2r == 1)
    {    
      vH2[i]=0; // Player 2 does not have her hand on Triangle i.
    }
    ///////////////////////////////////////////////////////



    // TRIANGLE 3 ///////////////////////////////////
    if ((arr3r == 0) && (vH3[i]==0))
    {
      midArr[3] = 1; // Player 3 has hit a note, set HI so that there is MIDI output for player 3
      //noteInd[3] = 0;  
      irInd[3] = i;   // Note to play for Player 3
      vH3[i]=1; // Player 3 has her hand on Triangle i. Set vH0 high so that note won't continue to play if Player 3 leaves hand on triangle i    
    }
    else if (arr3r == 1)
    {    
      vH3[i]=0; // Player 3 does not have her hand on Triangle i.
    }
    ///////////////////////////////////////////////////////


    // TRIANGLE 4 ///////////////////////////////////
    if ((arr4r == 0) && (vH4[i]==0))
    {
      midArr[4] = 1; // Player 4 has hit a note, set HI so that there is MIDI output for player 4
      //noteInd[4] = 0;  
      irInd[4] = i;   // Note to play for Player 4
      vH4[i]=1; // Player 4 has her hand on Triangle i. Set vH0 high so that note won't continue to play if Player 4 leaves hand on triangle i    
    }
    else if (arr4r == 1)
    {    
      vH4[i]=0; // Player 4 does not have her hand on Triangle i.
    }
    ///////////////////////////////////////////////////////
    

    PORTB ++; // increment MUX for next IR sensor

  }
  updatestring(dataPin);
  delay(70);
}

////////////////////////////////////
void updatestring(int ledS)
{
  
  for (int i = 0; i < numLeds; i++) // Shift output for LED strip
  {
    shiftOut(ledS, clockPin, MSBFIRST, red[i]);
    shiftOut(ledS, clockPin, MSBFIRST, green[i]);
    shiftOut(ledS, clockPin, MSBFIRST, blue[i]);
  }

  PORTB = B00000000; // SET control pins for mux low
  if (midArr[0] == 1) // Player 0 MIDI output
  {
    MIDI.sendNoteOn(notes[irInd[0]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[0]], 127, 1);
    delayMicroseconds(pauseMidi);
  }
  
  PORTB ++; // increment MUX
  if (midArr[1] == 1) // Player 1 MIDI output
  {
    MIDI.sendNoteOn(notes[irInd[1]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[1]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  PORTB ++; // increment MUX
  if (midArr[2] == 1) // Player 2 MIDI output
  {
    MIDI.sendNoteOn(notes[irInd[2]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[2]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  PORTB ++; // increment MUX
  if (midArr[3] == 1) // Player 3 MIDI output
  {
    MIDI.sendNoteOn(notes[irInd[3]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[3]], 127, 1);
    delayMicroseconds(pauseMidi);
  }

  PORTB ++; // increment MUX
  if (midArr[4] == 1) // Player 4 MIDI output
  {
    MIDI.sendNoteOn(notes[irInd[4]], 127, 1);
    delayMicroseconds(pauseMidi);
    MIDI.sendNoteOff(notes[irInd[4]], 127, 1);
    delayMicroseconds(pauseMidi);
  }
  
  midArr[0] = 0;
  midArr[1] = 0;
  midArr[2] = 0;
  midArr[3] = 0;
  midArr[4] = 0;

}
//////////////////////////////////
