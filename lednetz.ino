#include <FastLED.h>

#define NUM_LEDS 100
#define DATA_PIN 3
CRGB leds[NUM_LEDS];

int tonnetz[10][10];
int lednetz[10][10];
int orchestra[12][12];
int decayState[10][10];
float oscillator[10][10];
int sizeX = 10;
int member = 0;
char input;


unsigned int midi_offset = 48; //chooses the root note!- assign to dial  
unsigned int period = 50; //period between not chnages - assign to dial
unsigned int max_notes = 0; //number of simultaneous notes - as to dial

unsigned long previousMillis = 0;

unsigned long previousMillis_potentios = 0;
unsigned int potentio_poll_period = 1000;
unsigned long previousMillis_SerialInfo = 0;
unsigned int SerialInfo_poll_period = 5000;

const int analogInPin1 = A12;
int sensorValue1 = 0;        // value read from the pot 
const int analogInPin2 = A13;
int sensorValue2 = 0;
const int analogInPin3 = A14;
int sensorValue3 = 0;
const int analogInPin4 = A15;
int sensorValue4 = 0;
const int analogInPin5 = A16;
int sensorValue5 = 0;

void setup() {
  
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  for(int i = 0; i < sizeX; i = i + 1)
    {
      for(int j = 0; j < sizeX; j = j + 1)
      {
        
        tonnetz[i][j] = i * 3 + j * 4;
        //tonnetz[i][j] = tonnetz[i][j] + midi_offset;
        lednetz[i][j] = tonnetz[i][j] % 12;
        lednetz[i][j] = lednetz[i][j] * 20;
        
      }
    }

   for (int i = 0; i < sizeX + 2; i = i + 1)
    {
      for (int j = 0; j < sizeX + 2; j = j + 1)
      { 
        orchestra[i][j] = 0;
      }      
    }

    int init_x = random(0,12);
    int init_y = random(0,12);

    orchestra[init_x][init_y] = 1;

    //for (int i leds[1] = CRGB(50, 1, 1);
    //
    //FastLED.show();

}

void loop() 
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
      Serial.println("'0' tells all members to stop");
      Serial.println("'9' tells all members to play");
      Serial.println("'z' displays the current settings");
      Serial.println("'x' displays the note in MIDI of each orchestra member");
      Serial.println("'c' displays the state of all the members, '1' = On, '0' = Off");
    }
    if (input == 'q')
    {
      midi_offset = midi_offset + 1;
      Serial.println(midi_offset);
    }
    if (input == 'a')
    {
      midi_offset = midi_offset - 1;
      Serial.println(midi_offset);
    }
    if (input == 'w')
    {
      period = period + 1;
      Serial.println(period);
    }
    if (input == 's')
    {
      period = period - 1;
      Serial.println(period);
    }
    if (input == 'e')
    {
      max_notes = max_notes + 1;
      Serial.println(max_notes);
    }
    if (input == 'd')
    {
      max_notes = max_notes - 1;
      Serial.println(max_notes);
    }
    if (input == 'z')
    {
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
          int var = tonnetz[i][j]+midi_offset;
          Serial.print(var);
          Serial.print("\t");
        }
        Serial.println();
      }
      Serial.println();
    }
    if (input == 'c')
    {
      for(int i = 1; i < sizeX+1; i = i + 1)
      {
        for(int j = 1; j < sizeX+1; j = j + 1)
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
      orchestra[random(1,11)][random(1,11)] = 1;
    }
    if (input == '0')
    {// clear
      for(int i = 0; i < sizeX+2; i = i + 1)
      {
        for(int j = 0; j < sizeX+2; j = j + 1)
        {
          orchestra[i][j] = 0;
        }
      }
    }
    if (input == '9')
    {// fill
      for(int i = 0; i < sizeX+2; i = i + 1)
      {
        for(int j = 0; j < sizeX+2; j = j + 1)
        {
          orchestra[i][j] = 1;
        }
      }
      max_notes = (sizeX+1) * 4;
    }
    
  }
  
  unsigned long currentMillis = millis();

  //if (currentMillis - previousMillis_potentios >= potentio_poll_period) 
  if (currentMillis == 0)
  {
  previousMillis_potentios = currentMillis;
  
  sensorValue1 = analogRead(analogInPin1);
  midi_offset = map(sensorValue1, 0, 1023, 0, 120);

  sensorValue2 = analogRead(analogInPin2);
  period = map(sensorValue2, 0, 1023, 2, 500);

  sensorValue3 = analogRead(analogInPin3);
  max_notes = map(sensorValue3, 0, 1023, 0, 100);

  Serial.println(midi_offset);
  Serial.println(period);
  Serial.println(max_notes);
  Serial.println();
  
  }

  if (currentMillis - previousMillis_SerialInfo >= SerialInfo_poll_period) 
  {
    previousMillis_SerialInfo = currentMillis;
    //timed stuff
  }
  
  if (currentMillis - previousMillis >= period) 
  {
  previousMillis = currentMillis;

    member = random(0,100);
  
    int x_co = member % 10;
    int y_co = member / 10;
  
    if (y_co % 2 != 0)
    {
      x_co = 9 - x_co;
    }
  
    int or_x_co = x_co + 1;
    int or_y_co = y_co + 1;
  
    int total_playing = 0;
    for (int i = 1; i < sizeX; i = i + 1)
    {
      for (int j = 1; j < sizeX; j = j + 1)
      { 
        total_playing = total_playing + orchestra[i][j];
      }      
    } 
  
    if (orchestra[or_x_co][or_y_co] == 0)
    {
      if (orchestra[or_x_co-1][or_y_co] == 1 || orchestra[or_x_co][or_y_co-1] == 1 || orchestra[or_x_co+1][or_y_co] == 1 || orchestra[or_x_co][or_y_co+1] == 1 || orchestra[or_x_co+1][or_y_co+1] == 1 || orchestra[or_x_co+1][or_y_co-1] == 1 || orchestra[or_x_co-1][or_y_co+1] == 1 || orchestra[or_x_co-1][or_y_co-1] == 1)
      {
        if (total_playing <= max_notes)
        {
          //only play a note if it falls into on of the following scales:

//Western
          
          //major scale - root, whole (2semi/midi tones), whole, half (1semi/midi tone), whole, whole, whole, half
          if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 2) || (tonnetz[x_co][y_co] % 12 == 4) || (tonnetz[x_co][y_co] % 12 == 5)|| (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 9) || (tonnetz[x_co][y_co] % 12 == 11))
          
          //minor scale - root, W, h, W, W, h, W, W,
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 2) || (tonnetz[x_co][y_co] % 12 == 3) || (tonnetz[x_co][y_co] % 12 == 5) || (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 8) || (tonnetz[x_co][y_co] % 12 == 10)) 
 
          //Ionian Augmented - 2 2 1 3 1 2 1
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 2) || (tonnetz[x_co][y_co] % 12 == 4) || (tonnetz[x_co][y_co] % 12 == 5) || (tonnetz[x_co][y_co] % 12 == 8) || (tonnetz[x_co][y_co] % 12 == 9) || (tonnetz[x_co][y_co] % 12 == 11)) 

          //Gyspy Hexatonic - 1 3 1 2 1 1 3
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 1) || (tonnetz[x_co][y_co] % 12 == 4) || (tonnetz[x_co][y_co] % 12 == 5) || (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 8) || (tonnetz[x_co][y_co] % 12 == 9)) 
//Odds + Ends

          //root and octaves only
          //if ((tonnetz[x_co][y_co] % 12 == 0))

          //root and octaves & fifths only
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 7))

//Sexatonic

          //Messiaen 5th Mode - 1 4 1 1 4 1
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 1) || (tonnetz[x_co][y_co] % 12 == 5) || (tonnetz[x_co][y_co] % 12 == 6) || (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 11))
          
//Pentatonics

          //Pelog - 1 2 4 1 4
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 1) || (tonnetz[x_co][y_co] % 12 == 3) || (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 8))
          
          //Ritusen - 2 3 2 2 3
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 2) || (tonnetz[x_co][y_co] % 12 == 5) || (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 9))
          
          //pygmy - 2 1 4 3 2
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 2) || (tonnetz[x_co][y_co] % 12 == 3) || (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 10))

          //Raga Shailja - 3 4 1 2 2
          //if ((tonnetz[x_co][y_co] % 12 == 0) || (tonnetz[x_co][y_co] % 12 == 3) || (tonnetz[x_co][y_co] % 12 == 7) || (tonnetz[x_co][y_co] % 12 == 8) || (tonnetz[x_co][y_co] % 12 == 10))
          
          //play note:
          {
          orchestra[or_x_co][or_y_co] = 1;
          usbMIDI.sendNoteOn(tonnetz[x_co][y_co] + midi_offset, 99, 1);
          leds[member]= CHSV(lednetz[x_co][y_co], 255, 100);
          //Serial.println(tonnetz[x_co][y_co]);
          }
        }
      }  
    }
    else
    {
      orchestra[or_x_co][or_y_co] = 0;
      usbMIDI.sendNoteOff(tonnetz[x_co][y_co] + midi_offset, 99, 1);
      leds[member] = CRGB(1, 1, 1);
    }
    if (total_playing == 0) //reset if stuck/all off
    {
      orchestra[random(0,12)][random(0,12)] = 1;
    } 
    FastLED.show();
  }
}
