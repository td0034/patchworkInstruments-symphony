#include <FastLED.h>
#include <U8g2lib.h>
#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
// usb audio from PC channels 1 & 2 (L&R)
AudioInputUSB            usb2;           //xy=213,383
// 6.35mm mono input jacks 1 & 2 (L&R)
AudioInputI2S            i2s1;           //xy=216,228
// usb audio to PC 
AudioOutputUSB           usb1;           //xy=477,376
// 3.5mm stereo output jack
AudioOutputI2S           i2s2;           //xy=501,209
// virtual config 
AudioConnection          patchCord1(usb2, 0, i2s2, 0);
AudioConnection          patchCord2(usb2, 1, i2s2, 1);
AudioConnection          patchCord3(i2s1, 0, usb1, 0);
AudioConnection          patchCord4(i2s1, 1, usb1, 1);
// audio shield 
AudioControlSGTL5000     sgtl5000_1;     //xy=317,450
// GUItool: end automatically generated code



#define NUM_LEDS_PER_STRIP  100
#define NUM_STRIPS          1
#define NUM_LEDS            NUM_LEDS_PER_STRIP * NUM_STRIPS
#define DATA_PIN            0
#define BRIGHTNESS          48
#define LAST_VISIBLE_LED    90

// init fastled obj
CRGB leds[ NUM_LEDS ];

//init lcd screen obj
U8G2_SSD1327_MIDAS_128X128_F_4W_HW_SPI u8g2(U8G2_R0,/* cs=*/ 41, /*miso dc=*/ 40 ) ;

// init tonnetz 
int tonnetz[11][11];
int midinetz[11][11];
int sizeX = 11;

// init single letter serial command variable
char input;

// init orchestra
int orchestra[13][13];
int lastOrchestra[13][13];

//init buttons
const int push1 = 22;
const int push2 = 1;
const int ENC_SWITCH = 3;
int param = 0;

// Params for width and height
const uint8_t kMatrixWidth = 13;
const uint8_t kMatrixHeight = 13;

unsigned int midi_offset = 37; //chooses the root note!- assign to dial  
unsigned int period = 50; //period between not chnages - assign to dial
unsigned int max_notes = 5; //number of simultaneous notes - as to dial

unsigned long previousMillis = 0;

//loop variables
int loopXY[2][2][10000];
int maxLoopLength = 10000;
int loopLength = 1000;
int loopPosition = 0;
bool recordPlay = 0;

//debug variables
int counter = 0;
int xD = 1;
int yD = 1;

// Variables added by Alex

int hue = 0;
int bright = 0;
int x_co = 0;
int y_co = 0;
int note = 0;
unsigned int total_playing = 0;
int totalBright = 0;
unsigned long currentMillis = 0;
// scale stuff
int scale = 12;

int scale0 = 0;
int scale1 = 0;
int scale2 = 0;
int scale3 = 0;
int scale4 = 0;
int scale5 = 0;
int scale6 = 0;
int scale7 = 0;
int scale8_ = 0;
int scale9 = 0;
int scale10 = 0;
int scale11 = 0;

int s0 = 0;
int s1 = 0;
int s2 = 0;
int s3 = 0;
int s4 = 0;
int s5 = 0;
int s6 = 0;
int s7 = 0;
int s8 = 0;
int s9 = 0;
int s10 = 0;
int s11 = 0;

int totalIntervals = 0;

// encoder stuff
Encoder encoder1 (3, 18);
Encoder encoder2 (16, 15);
Encoder encoder3 (3, 4);
Encoder encoder4 (28, 29);
int oldPosition1 =0;
int oldPosition2 =0;
int oldPosition3 =0;
int oldPosition4 =0;

// to allow the commmit button, alt variables for each parameter
unsigned int altScale = scale;
unsigned int altMidi_offset = midi_offset;
unsigned int altPeriod = period;
unsigned int altMax_notes = max_notes;

//        I N T E R R U P T S
long position0 = -999;   
long position1  = -999;
long position2 = -999;
long position3  = -999;
long position4 = -999;

// This function sets up the ledsand tells the controller about them
void setup() 
{
  AudioMemory(12);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  
  // init pushbuttons
  pinMode(push1,INPUT_PULLUP);
  pinMode(push2,INPUT_PULLUP);
  pinMode(ENC_SWITCH, INPUT);

  pinMode(14,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(37,INPUT_PULLUP);
  pinMode(36,INPUT_PULLUP);
  pinMode(35,INPUT_PULLUP);
  pinMode(34,INPUT_PULLUP);
  pinMode(33,INPUT_PULLUP);
  pinMode(32,INPUT_PULLUP);
  pinMode(31,INPUT_PULLUP);
  pinMode(30,INPUT_PULLUP);
  pinMode(38,INPUT_PULLUP);
  pinMode(39,INPUT_PULLUP);

  attachInterrupt(push2, paramSelect, FALLING);

  //attachInterrupt(digitalPinToInterrupt(push1), inc, FALLING);


  // setting up screen
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14_tf);
  u8g2.setCursor(22, 22);  
  u8g2.print("Pi S3A 002");
  u8g2.sendBuffer();

  
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
 	delay(2000);
  FastLED.addLeds<NUM_STRIPS, WS2811, DATA_PIN, RGB>(leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.clear();

  setUpTonnetz();
  clearOrchestra();
  tickleOrchestra();

  for(int yl = 0; yl < 13; yl = yl + 1)
  {   
   for(int xl = 0; xl < 13; xl = xl + 1)
   {  
      if (XY(xl,yl)<91)
      {
        leds[XY(xl,yl)]= CHSV(tonnetz[xl-1][yl-1], 255, 150);
      }      
   }
  }
  
  FastLED.show();
  delay(1000);
  FastLED.clear();

  for (int n = 0; n<2; n = n +1)
  {
    for (int i = 0; i<2; i = i+1)
    {
      for (int j = 0; j < maxLoopLength; j = j+1)
      {
        loopXY[n][i][j] = 0;
      }
    }
  }



}   

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop() 
{
  currentMillis = millis();
  
  checkSerial();                                        // original serial commands
  
  checkInputs();                                        // check buttons
  
  if (currentMillis - previousMillis >= period)         // this timing can be ellaborated on for quantising
  {
    previousMillis = currentMillis;

    COUNT_ORCHESTRA_MEMBERS();                          // count number of playing orchestra members
    
    if (total_playing <= max_notes)
    {
      if (recordPlay == 0)
      { 
      //Serial.println("Recording Note On");
      HEX_TNZ_NEIGHBOURS_SCALE_ON(loopPosition);     
      }// tell a random orchestra member to play
      
      if (recordPlay == 1)
      {
        //Serial.print("Playing sample :");
        //Serial.println(loopPosition);
        HEX_TNZ_NEIGHBOURS_SCALE_LOOP_ON(loopPosition);
      }
    }

    if (recordPlay == 0)
    { 
      //Serial.println("Recording Note Off");
      HEX_TNZ_NEIGHBOURS_OFF(loopPosition);
    }// tell a random orchestra member to stop
    
    if (recordPlay == 1)
    {
      HEX_TNZ_NEIGHBOURS_LOOP_OFF(loopPosition);
    }

    if (total_playing < 1)
    {
      tickleOrchestra();                                // tickle a random orchestra member if none are playing
    }
    
    if (loopPosition >= loopLength)
    {
      //recordPlay = 1;
      loopPosition = 0;
    }

    //Serial.println(recordPlay);
    

    loopPosition = loopPosition + 1;
    
  }

  screen();                                               // updates the screen

  FastLED.show();                                         // updates the lights


}


uint8_t XY (uint8_t x, uint8_t y)                         // maps the light index numbers (0 to 90) to a hexagonal grid
{
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) 
  {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t XYTable[] = 
  {

    103,   104,  116,  117,  125,  126,  130,  131,  137,  138,  148,  149,  168, 
    102,  0,  12,   13,   29,   30,   50,   132,  136,  139,  147,  150,  167, 
    101,  1,  11,   14,   28,   31,   49,   51,   135,  140,  146,  151,  166, 
    100,  2,  10,   15,   27,   32,   48,   52,   69,   141,  145,  152,  165, 
    99,   3,  9,  16,   26,   33,   47,   53,   68,   70,   144,  153,  164, 
    98,   4,  8,  17,   25,   34,   46,   54,   67,   71,   84,   154,  163, 
    97,   5,  7,  18,   24,   35,   45,   55,   66,   72,   83,   85,   162, 
    96,   105,  6,  19,   23,   36,   44,   56,   65,   73,   82,   86,   161, 
    95,   106,  115,  20,   22,   37,   43,   57,   64,   74,   81,   87,   160, 
    94,   107,  114,  118,  21,   38,   42,   58,   63,   75,   80,   88,   159, 
    93,   108,  113,  119,  124,  39,   41,   59,   62,   76,   79,   89,   158, 
    92,   109,  112,  120,  123,  127,  40,   60,   61,   77,   78,   90,   157, 
    91,   110,  111,  121,  122,  128,  129,  133,  134,  142,  143,  155,  156 
  
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

void setUpTonnetz()                                       // sets up an array with midi equivalents of Eulers Tonnetz grid
{
  for(int i = 0; i < sizeX; i = i + 1)
  {
    for(int j = 0; j < sizeX; j = j + 1)
    {
      
      tonnetz[i][j] = i * 3 + j * 4;                      // simplicity!
      midinetz[i][j] = tonnetz[i][j];
      tonnetz[i][j] = tonnetz[i][j] % 12;                 // western keyboard wraps around every 12 notes
      
      usbMIDI.sendNoteOff(i*3, 99, 1);                    // turn off all midi notes at startup to avoid hanging notes
      usbMIDI.sendNoteOff(j*4, 99, 1);
    }
  }
}

void clearOrchestra()                                     // tell all orchestra members to stop playing
{
  for (int i = 0; i < sizeX + 2; i = i + 1)
  {
    for (int j = 0; j < sizeX + 2; j = j + 1)
    { 
      orchestra[i][j] = 0;
    }      
  }
}

void tickleOrchestra()
{
  int init_x = random(0,12);
  int init_y = random(0,12);

  if (XY(init_x,init_y)<91)
  {
    selectScale();                                        // only tickle an orchestra member if they are within the current scale
    if (
    (midinetz[init_x-1][init_y-1] % 12 == scale0) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale1) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale2) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale3) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale4) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale5) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale6) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale7) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale8_) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale9) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale10) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale11)
       )
    {
      orchestra[init_x][init_y] = 1;
    }
  }
}

void checkInputs()                                        // check the rotary encoders and pushbuttons
{
   long new1;
  new1 = encoder4.read();
  //Serial.println(new1);
  //new2 = encoder2.read();
  //new3 = encoder3.read();
  //new4 = encoder4.read();
  if (new1 != position0)
  {
    position0 = new1;
  }

  if(param == 0){position1 = position0;}
  if(param == 1){position2 = position0;}
  if(param == 2){position3 = position0;}
  if(param == 3){position4 = position0;}
  
  if (position1 - oldPosition1 > 3){altScale++;oldPosition1 = position1;}
  if (oldPosition1 - position1 > 3){altScale--;oldPosition1 = position1;}
  if (altScale<0){altScale=10;}
  if (altScale>99){altScale=0;}
  
  if (position2 - oldPosition2 > 3){altMidi_offset++;oldPosition2 = position2;}
  if (oldPosition2 - position2 > 3){altMidi_offset--;oldPosition2 = position2;}
  if (altMidi_offset<0){altMidi_offset=127;}
  if (altMidi_offset>127){altMidi_offset=0;}
  
  if (position3 - oldPosition3 > 3){altPeriod++;oldPosition3 = position3;}
  if (oldPosition3 - position3 > 3){altPeriod--;oldPosition3 = position3;}
  if (altPeriod<0){altPeriod=999;}
  if (altPeriod>999){altPeriod=0;}
  
  if (position4 - oldPosition4 > 3){altMax_notes++;oldPosition4 = position4;}
  if (oldPosition4 - position4 > 3){altMax_notes--;oldPosition4 = position4;}
  if (altMax_notes<0){altMax_notes=999;}
  if (altMax_notes>999){altMax_notes=0;}
  
  if (digitalRead(push1) == LOW)
  {
    RESET();
  }
  if (digitalRead(ENC_SWITCH) == HIGH)
  {
    scale = altScale;
    midi_offset = altMidi_offset;
    period = altPeriod;
    max_notes = altMax_notes;
  }
  
  
}

void screen()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14_mf);

  u8g2.setCursor(0, 20);  
  u8g2.print("Patchwork S3A");

  u8g2.setCursor(0, 38);  
  if(altScale!=scale){u8g2.print("~");}
  u8g2.setCursor(10, 34);
  typeScale();
  
  u8g2.setCursor(0, 48);  
  u8g2.print("Root Note");
  u8g2.setCursor(94, 52);  
  if(altMidi_offset!=midi_offset){u8g2.print("~");}
  u8g2.setCursor(104, 48);  
  typeRoot();
  
  u8g2.setCursor(0, 62);  
  u8g2.print("Period");
  u8g2.setCursor(94, 66);  
  if(altPeriod!=period){u8g2.print("~");}
  u8g2.setCursor(104, 62); 
  u8g2.print(altPeriod);
  
  u8g2.setCursor(0, 76);  
  u8g2.print("Max Notes");
  u8g2.setCursor(94, 80);  
  if(altMax_notes!=max_notes){u8g2.print("~");}
  u8g2.setCursor(104, 76);  
  u8g2.print(altMax_notes);
  u8g2.sendBuffer();
}

void typeScale()
{
  if(altScale==0){u8g2.print("Major");}
  if(altScale==1){u8g2.print("Minor");}
  if(altScale==2){u8g2.print("Ionian Augmented");}
  if(altScale==3){u8g2.print("Gyspy Hexatonic");}
  if(altScale==4){u8g2.print("Root");}
  if(altScale==5){u8g2.print("Root and 5th");}
  if(altScale==6){u8g2.print("Messiaen 5th Mode");}
  if(altScale==7){u8g2.print("Pelog");}
  if(altScale==8){u8g2.print("Ritusen");}
  if(altScale==9){u8g2.print("Pygmy");}
  if(altScale==10){u8g2.print("Raga Shailja");}
  if(altScale==11){u8g2.print("Switch Input");}
  if(altScale==12){u8g2.print("Bismillahi");}
}

void typeRoot()
{
  if(altMidi_offset%12 == 0){u8g2.print("C");}
  if(altMidi_offset%12 == 1){u8g2.print("C#");}
  if(altMidi_offset%12 == 2){u8g2.print("D");}
  if(altMidi_offset%12 == 3){u8g2.print("Eb");}
  if(altMidi_offset%12 == 4){u8g2.print("E");}
  if(altMidi_offset%12 == 5){u8g2.print("F");}
  if(altMidi_offset%12 == 6){u8g2.print("F#");}
  if(altMidi_offset%12 == 7){u8g2.print("G");}
  if(altMidi_offset%12 == 8){u8g2.print("Ab");}
  if(altMidi_offset%12 == 9){u8g2.print("A");}
  if(altMidi_offset%12 == 10){u8g2.print("Bb");}
  if(altMidi_offset%12 == 11){u8g2.print("B");}
  u8g2.setCursor(118, 48);
  if(altMidi_offset/12 == 0){u8g2.print("0");}
  if(altMidi_offset/12 == 1){u8g2.print("1");}
  if(altMidi_offset/12 == 2){u8g2.print("2");}
  if(altMidi_offset/12 == 3){u8g2.print("3");}
  if(altMidi_offset/12 == 4){u8g2.print("4");}
  if(altMidi_offset/12 == 5){u8g2.print("5");}
  if(altMidi_offset/12 == 6){u8g2.print("6");}
  if(altMidi_offset/12 == 7){u8g2.print("7");}
  if(altMidi_offset/12 == 8){u8g2.print("8");}
}


void selectScale()
{
  
//                              W E S T E R N
  //major scale
  if (scale == 0){ 
    scale1 = 0;
    scale2 = 2;
    scale3 = 4;
    scale4 = 5;
    scale5 = 7;
    scale6 = 9;
    scale7 = 11;
    }

  //minor scale
  if (scale == 1){ 
    scale1 = 0;
    scale2 = 2;
    scale3 = 3;
    scale4 = 5;
    scale5 = 7;
    scale6 = 8;
    scale7 = 10;
    }
    
  //Ionian Augmented - 2 2 1 3 1 2 1
  if (scale == 2){ 
    scale1 = 0;
    scale2 = 2;
    scale3 = 4;
    scale4 = 5;
    scale5 = 8;
    scale6 = 9;
    scale7 = 11;
    }

   //Gyspy Hexatonic - 1 3 1 2 1 1 3
     if (scale == 3){ 
    scale1 = 0;
    scale2 = 1;
    scale3 = 4;
    scale4 = 5;
    scale5 = 7;
    scale6 = 8;
    scale7 = 9;
    }


//                             O D D S  +  E N D S 
    //root and octaves only
    if (scale == 4){ 
    scale1 = 0;
    scale2 = 0;
    scale3 = 0;
    scale4 = 0;
    scale5 = 0;
    scale6 = 0;
    scale7 = 0;
    }


    //root and octaves & fifths only
    if (scale == 5){ 
    scale1 = 0;
    scale2 = 7;
    scale3 = 0;
    scale4 = 0;
    scale5 = 0;
    scale6 = 0;
    scale7 = 0;
    }


//                             S E X A T O N I C
    //Messiaen 5th Mode - 1 4 1 1 4 1
    if (scale == 6){ 
    scale1 = 0;
    scale2 = 1;
    scale3 = 5;
    scale4 = 6;
    scale5 = 7;
    scale6 = 11;
    scale7 = 0;
    }


//                             P E N T A T O N I C S
    //Pelog - 1 2 4 1 4
    if (scale == 7){ 
    scale1 = 0;
    scale2 = 1;
    scale3 = 3;
    scale4 = 7;
    scale5 = 8;
    scale6 = 0;
    scale7 = 0;
    }

    //Ritusen - 2 3 2 2 3
    if (scale == 8){ 
    scale1 = 0;
    scale2 = 2;
    scale3 = 5;
    scale4 = 7;
    scale5 = 9;
    scale6 = 0;
    scale7 = 0;
    }

    //Pygmy - 2 1 4 3 2
    if (scale == 9){ 
    scale1 = 0;
    scale2 = 2;
    scale3 = 3;
    scale4 = 7;
    scale5 = 10;
    scale6 = 0;
    scale7 = 0;
    }

    //Raga Shailja - 3 4 1 2 2
    if (scale == 10){ 
    scale1 = 0;
    scale2 = 3;
    scale3 = 7;
    scale4 = 8;
    scale5 = 10;
    scale6 = 0;
    scale7 = 0;
    }

    //Bismillahi - C# root - 1 1 5 1 1 3
    if (scale == 12){ 
    scale1 = 0;
    scale2 = 2;
    scale3 = 5;
    scale4 = 7;
    scale5 = 9;
    scale6 = 0;
    scale7 = 0;
    }


    //serial input scale with characters from 1 (C) to - (B)
    if (scale == 11)
    {
      scale0 = 0;
       
      if (digitalRead(14) == LOW)
      {
        scale1 = 1;  
      }
      else
      {
        scale1 = 0;
      }
      if (digitalRead(9) == LOW)
      {
        scale2 = 2;
      }
      else
      {
        scale2 = 0;
      }
      if (digitalRead(39) == LOW)
      {
        scale3 = 3;
      }
      else
      {
        scale3 = 0;
      }
      if (digitalRead(38) == LOW)
      {
        scale4 = 4;
      }
      else
      {
        scale4 = 0;
      }
      if (digitalRead(37) == LOW)
      {
        scale5 = 5;
      }
      else
      {
        scale5 = 0;
      }
      if (digitalRead(36) == LOW)
      {
        scale6 = 6;
      }
      else
      {
        scale6 = 0;
      }
      if (digitalRead(32) == LOW)
      {
        scale7 = 7;
      }
      else
      {
        scale7 = 0;
      }
      if (digitalRead(31) == LOW)
      {
        scale8_ = 8;
      }
      else
      {
        scale8_ = 0;
      }
      if (digitalRead(30) == LOW)
      {
        scale9 = 9;
      }
      else
      {
        scale9 = 0;
      }
      if (digitalRead(33) == LOW)
      {
        scale10 = 10;
      }
      else
      {
        scale10 = 0;
      }
      if (digitalRead(34) == LOW)
      {
        scale11 = 11;
      }
      else
      {
        scale11 = 0;
      }
    }
    
} // end selectScale


// ---------------------------------------------------------------------
void hueFromMidi(int x, int y)
{
  note = tonnetz[x-1][y-1] + midi_offset;
  if (note % 12 == 0){hue = 0;}     //red
  if (note % 12 == 1){hue = 21;}    //orange
  if (note % 12 == 2){hue = 43;}    // ...
  if (note % 12 == 3){hue = 64;}
  if (note % 12 == 4){hue = 85;}
  if (note % 12 == 5){hue = 106;}
  if (note % 12 == 6){hue = 128;}
  if (note % 12 == 7){hue = 149;}
  if (note % 12 == 8){hue = 170;}
  if (note % 12 == 9){hue = 191;}
  if (note % 12 == 10){hue = 213;}  // ...
  if (note % 12 == 11){hue = 234;}  //magenta
}

void brightnessFromTotalMembers()
{
 totalBright = total_playing;
 if(total_playing >= 10){totalBright = 10;} // rarely reach more than 10 playing so it might aswell be the limit
 bright = map(totalBright, 0, 10, 40, 220); // newValue = map(oldValue, fromLow, fromHi, toLow, toHigh)
}

void checkSerial()
{
  //Serial commands  
  if(Serial.available())
  {
    input = Serial.read();
    Serial.print("");
    Serial.println(input);

    if (input == 'h')
    {
      Serial.println("Help:");
      Serial.println("'q' increases root note by 1 semi-tone, 'a' decreases by 1 semi-tone");
      Serial.println("'w' increases orchestra update period by 1 microsecond, 's' decreases");
      Serial.println("'e' adds 1 to the number of simultaneous orchestra members, 'd' decreases");
      Serial.println("'t' tickles a random orchestra member");
      //Serial.println("'0' tells all members to stop");
      //Serial.println("'9' tells all members to play");
      Serial.println("'z' displays the current settings");
      Serial.println("'x' displays the note in MIDI of each orchestra member");
      Serial.println("'c' displays the state of all the members, '1' = On, '0' = Off");
    }
    if (input == 'q'){midi_offset = midi_offset + 1;Serial.println(midi_offset);}
    if (input == 'a'){midi_offset = midi_offset - 1;Serial.println(midi_offset);}
    if (input == 'w'){period = period + 1;Serial.println(period);}
    if (input == 's'){period = period - 1;Serial.println(period);}
    if (input == 'e'){max_notes = max_notes + 1;Serial.println(max_notes);}
    if (input == 'd'){max_notes = max_notes - 1;Serial.println(max_notes);}
    if (input == 'z'){
      Serial.print("Midi Offset: ");
      Serial.print(midi_offset);
      Serial.print(" - Period: ");
      Serial.print(period);
      Serial.print(" - Max Notes: ");
      Serial.println(max_notes);
    }
    if (input == 'x')
    {
      for(int i = 0; i < sizeX; i = i + 1)
      {
        for(int j = 0; j < sizeX; j = j + 1)
        {
          int var = tonnetz[i][j];
          Serial.print(var);
          Serial.print("\t");
        }
        Serial.println();
      }
      Serial.println();
    }
    if (input == 'v')
    {
      for(int i = 0; i < sizeX; i = i + 1)
      {
        for(int j = 0; j < sizeX; j = j + 1)
        {
          int var = midinetz[i][j]+midi_offset;
          Serial.print(var);
          Serial.print("\t");
        }
        Serial.println();
      }
      Serial.println();
    }
    if (input == '`')
    {
      for(int i = 0; i < 13; i = i + 1)
      {
        for(int j = 0; j < 13; j = j + 1)
        {
          //int var = tonnetz[i][j] + midi_offset;
          int var = XY(i,j);
          Serial.print(var);
          Serial.print("\t");
        }
        Serial.println();
      }
      Serial.println();
    }
    if (input == 'c')
    {
      for(int i = 0; i < 13; i = i + 1)
      {
        for(int j = 0; j < 13; j = j + 1)
        {
          int var = orchestra[i][j];
          Serial.print(var);
          Serial.print("\t");
        }
        Serial.println();
      }
      Serial.println();
    }
    if (input == 't')
    {// tickle
      tickleOrchestra();
    }

    if (input == ',')
    {// loopLength down  
      loopLength = loopLength - 1;
      Serial.println(loopLength);
    }
    
    if (input == '.')
    {// loopLength up  
      loopLength = loopLength + 1;
      Serial.println(loopLength);
    }

    if (input == 'r')
    {
      recordPlay = !recordPlay;
      Serial.println(recordPlay);
    }
    
    //scale setting C = 1, C# = 2, ..., A = 9, A# = 0, B = "-" 
    if (input == '1')
    {
      s1 = s1 + 1;
      
      if (s1 > 11)
      {
        s1 = 0; 
      }
      Serial.println(s1);
    }
    if (input == '2')
    {
      s2 = s2 + 1;
 
      if (s2 > 11)
      {
        s2 = 0; 
      }
      Serial.println(s2);
    }
    if (input == '3')
    {
      s3 = s3 + 1;
 
      if (s3 > 11)
      {
        s3 = 0; 
      }
      Serial.println(s3);
    }
    if (input == '4')
    {
      s4 = s4 + 1;
 
      if (s4 > 11)
      {
        s4 = 0; 
      }
      Serial.println(s4);
    }
    if (input == '5')
    {
      s5 = s5 + 1;
 
      if (s5 > 11)
      {
        s5 = 0; 
      }
      Serial.println(s5);
    }
    if (input == '6')
    {
      s6 = s6 + 1;
 
      if (s6 > 11)
      {
        s6 = 0; 
      }
      Serial.println(s6);
    }
    if (input == '7')
    {
      s7 = s7 + 1;
 
      if (s7 > 11)
      {
        s7 = 0; 
      }
      Serial.println(s7);
    }
    if (input == '8')
    {
      s8 = s8 + 1;
 
      if (s8 > 11)
      {
        s8 = 0; 
      }
      Serial.println(s8);
    }
    if (input == '9')
    {
      s9 = s9 + 1;
 
      if (s9 > 11)
      {
        s9 = 0; 
      }
      Serial.println(s9);
    }
    if (input == '0')
    {
      s10 = s10 + 1;
 
      if (s10 > 11)
      {
        s10 = 0; 
      }
      Serial.println(s10);
    }
    if (input == '-')
    {
      s11 = s11 + 1;
 
      if (s11 > 11)
      {
        s11 = 0; 
      }
      Serial.println(s11);
    }
    if (input == '=')
    {
      s1 = s1;
      s2 = s1 + s2;
      s3 = s2 + s3;
      s4 = s3 + s4;
      s5 = s4 + s5;
      s6 = s5 + s6;
      s7 = s6 + s7;
      s8 = s7 + s8;
      s9 = s8 + s9;
      s10 = s9 + s10;
      s11 = s10 + s11;
      
      
      Serial.print("Scale: ");
      Serial.print(s1);
      Serial.print(" ");
      Serial.print(s2);
      Serial.print(" ");
      Serial.print(s3);
      Serial.print(" ");
      Serial.print(s4);
      Serial.print(" ");
      Serial.print(s5);
      Serial.print(" ");
      Serial.print(s6);
      Serial.print(" ");
      Serial.print(s7);
      Serial.print(" ");
      Serial.print(s8);
      Serial.print(" ");
      Serial.print(s9);
      Serial.print(" ");
      Serial.print(s10);
      Serial.print(" ");
      Serial.println(s11);
    }
    if (input == '[')
    {
      s1 = 0;
      s2 = 0;
      s3 = 0;
      s4 = 0;
      s5 = 0;
      s6 = 0;
      s7 = 0;
      s8 = 0;
      s9 = 0;
      s10 = 0;
      s11 = 0;
      Serial.println("Reset Scale");
    }
    if (input == ']')
    {
      Serial.print("Scale: ");
      Serial.print(s1);
      Serial.print(" ");
      Serial.print(s2);
      Serial.print(" ");
      Serial.print(s3);
      Serial.print(" ");
      Serial.print(s4);
      Serial.print(" ");
      Serial.print(s5);
      Serial.print(" ");
      Serial.print(s6);
      Serial.print(" ");
      Serial.print(s7);
      Serial.print(" ");
      Serial.print(s8);
      Serial.print(" ");
      Serial.print(s9);
      Serial.print(" ");
      Serial.print(s10);
      Serial.print(" ");
      Serial.println(s11);
    }

    
  } //end serial check
}

void FX1()
{
  for(int yl = 0; yl < 13; yl = yl + 1)
  {   
   for(int xl = 0; xl < 13; xl = xl + 1)
   {  
      if (XY(xl,yl)<91)
      {
        int hueFx = tonnetz[xl-1][yl-1];
        hueFx = hueFx + xl;
        leds[XY(xl,yl)]= CHSV(hueFx, 255, 150);
      }      
   }
  }
  FastLED.show();
}

void DEBUG1()
{
  leds[90]= CHSV(0, 255, 150);
}

void RED_ON(int member)
{
  leds[member]= CHSV(0, 255, 150);
}

void LED_OFF(int member)
{
  leds[member]= CHSV(0, 0, 0);
}

void HEX_RED_ON(int x, int y)
{
  leds[XY(x,y)]= CHSV(0, 255, 150);
}

void HEX_LED_OFF(int x, int y)
{
  leds[XY(x,y)]= CHSV(0, 0, 0);
}

void HEX_TNZ_ON(int x, int y)
{
  hueFromMidi(x,y);
  leds[XY(x,y)]= CHSV(hue,255,200);
}

void HEX_TNZ_OFF(int x, int y)
{
  leds[XY(x,y)]= CHSV(tonnetz[x-1][y-1],255,0);
}

void HEX_TNZ_WHITE(int x, int y)
{
  leds[XY(x,y)]= CRGB(6, 6, 6);
}

void HEX_TNZ_RANDOM_ONOFF()
{
  int xD2 = random(1,12);
  int yD2 = random(1,12);

  if (XY(xD2,yD2)<91)
  {
    HEX_TNZ_ON(xD2, yD2);
    HEX_TNZ_MIDI_ON(xD2, yD2);
  }

  int xD3 = random(1,12);
  int yD3 = random(1,12);

  if (XY(xD3,yD3)<91)
  {
    HEX_TNZ_OFF(xD3, yD3);
    HEX_TNZ_MIDI_OFF(xD2, yD2);
  }
}

void HEX_TNZ_NEIGHBOURS_SCALE_ON(int loopPosition)
{
  int xD2 = random(1,12);
  int yD2 = random(1,12);

  loopXY[0][0][loopPosition] = xD2;
  loopXY[1][0][loopPosition] = yD2;

  if (XY(xD2,yD2)<91)
  {
    if (orchestra[xD2][yD2] == 0)
    {
      if (
      (orchestra[xD2-1][yD2] == 1) ||     // A, is the note 3 semitones below on?
      (orchestra[xD2+1][yD2] == 1) ||     // D, is the note 3 semitones above on?
      (orchestra[xD2-1][yD2-1] == 1) ||   // B, is the note 7 semitones below on?
      (orchestra[xD2+1][yD2+1] == 1) ||   // E, is the note 7 semitones above on?
      (orchestra[xD2][yD2+1] == 1) ||     // C, is the note 4 semitones below on?
      (orchestra[xD2][yD2-1] == 1) ||     // F, is the note 4 semitones above on?
      (orchestra[xD2-1][yD2+1] == 1) ||   // B, is the note 1 semitone below on?
      (orchestra[xD2+1][yD2-1] == 1)      // E, is the note 1 semitone above on?
         )
      {
        selectScale();
        if (
        (midinetz[xD2-1][yD2-1] % 12 == scale0) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale1) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale2) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale3) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale4) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale5) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale6) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale7) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale8_) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale9) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale10) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale11)
           )
        {
          HEX_TNZ_ON(xD2, yD2);
          HEX_TNZ_MIDI_ON(xD2, yD2);
          orchestra[xD2][yD2] = 1;
        }
      }
    }
  }
}

void HEX_TNZ_NEIGHBOURS_OFF(int loopPosition)
{
  int xD3 = random(1,12);
  int yD3 = random(1,12);

  loopXY[0][0][loopPosition] = xD3;
  loopXY[0][1][loopPosition] = yD3;

  if (XY(xD3,yD3)<91)
  {
    if (orchestra[xD3][yD3] == 1)
    {
      //HEX_TNZ_OFF(xD3, yD3);
      HEX_TNZ_WHITE(xD3, yD3);
      HEX_TNZ_MIDI_OFF(xD3, yD3);
      orchestra[xD3][yD3] = 0;
    }
  }
}

void HEX_TNZ_NEIGHBOURS_SCALE_LOOP_ON(int loopPosition)
{
  int xD2 = loopXY[0][0][loopPosition];
  int yD2 = loopXY[0][1][loopPosition];

  if (XY(xD2,yD2)<91)
  {
    if (orchestra[xD2][yD2] == 0)
    {
      if (
      (orchestra[xD2-1][yD2] == 1) ||     // A, is the note 3 semitones below on?
      (orchestra[xD2+1][yD2] == 1) ||     // D, is the note 3 semitones above on?
      (orchestra[xD2-1][yD2-1] == 1) ||   // B, is the note 7 semitones below on?
      (orchestra[xD2+1][yD2+1] == 1) ||   // E, is the note 7 semitones above on?
      (orchestra[xD2][yD2+1] == 1) ||     // C, is the note 4 semitones below on?
      (orchestra[xD2][yD2-1] == 1) ||     // F, is the note 4 semitones above on?
      (orchestra[xD2-1][yD2+1] == 1) ||   // B, is the note 1 semitone below on?
      (orchestra[xD2+1][yD2-1] == 1)      // E, is the note 1 semitone above on?
         )
      {
        selectScale();
        if (
        (midinetz[xD2-1][yD2-1] % 12 == scale0) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale1) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale2) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale3) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale4) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale5) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale6) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale7) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale8_) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale9) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale10) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale11)
           )
        {
          HEX_TNZ_ON(xD2, yD2);
          HEX_TNZ_MIDI_ON(xD2, yD2);
          orchestra[xD2][yD2] = 1;
        }
      }
    }
  }
}

void HEX_TNZ_NEIGHBOURS_LOOP_OFF(int loopPosition)
{
  int xD3 = loopXY[0][0][loopPosition];
  int yD3 = loopXY[1][0][loopPosition];

  if (XY(xD3,yD3)<91)
  {
    if (orchestra[xD3][yD3] == 1)
    {
      //HEX_TNZ_OFF(xD3, yD3);
      HEX_TNZ_WHITE(xD3, yD3);
      HEX_TNZ_MIDI_OFF(xD3, yD3);
      orchestra[xD3][yD3] = 0;
    }
  }
}

void HEX_TNZ_MIDI_ON(int x, int y)
{
  usbMIDI.sendNoteOn(midinetz[x-1][y-1] + midi_offset, 99, 1);
}

void HEX_TNZ_MIDI_OFF(int x, int y)
{
  usbMIDI.sendNoteOff(midinetz[x-1][y-1] + midi_offset, 99, 1);
}


void COUNT_ORCHESTRA_MEMBERS()
{
  total_playing = 0;
  for (int i = 1; i < 13; i = i + 1)
  {
    for (int j = 1; j < 13; j = j + 1)
    { 
      if (XY(i,j)<91)
      {
        total_playing = total_playing + orchestra[i][j]; // working out how many people playing
      }
    }      
  }
  //Serial.println(total_playing); 
}

void RESET()
{
  //clear orchestra
  for (int i = 1; i < 13; i = i + 1)
  {
    for (int j = 1; j < 13; j = j + 1)
    { 
      orchestra[i][j] = 0;
    }
  }
  
  //clear midi
  for (int i = 0; i < 127; i = i + 1)
  {
    //for (int j = 0; j < 12; j = j + 1)
    //{ 
    usbMIDI.sendNoteOff(i, 99, 1);
    //}
  }

   
  
  //clear lights
  FastLED.clear();

  //set recording back to on
  recordPlay = 0;
}

void paramSelect()
{    
  param = param + 1;
    if (param > 6)
    {
      param = 0;
    }
    Serial.println(param);
}

void inc()
{;
}
