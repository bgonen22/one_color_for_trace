#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// Which pin on the Arduino is connected to the NeoPixels?

#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60

// How many colors do you want (more colors, more gradient)?
#define NUMOFCOLORS      7


// jump between to traces (head trace to head trace - min JUMP = 3)
#define JUMP 3

// delay between iterations
int delayval = 100; // delay for half a second

// light level 1=255
float HIGHLEVEL=0.4;
float MEDIUMLEVEL=0.2;
float LOWLEVEL=0.06;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int head_color = 0;

void setup() {
 Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  int i = -1;

  while(1){
    i++;
//    Serial.println(head_color);
    if (i == NUMPIXELS-1) {
      i = LastLed(i);
      continue;    
    }      
    //Serial.println(i);
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    lightAllTraces(i, HIGHLEVEL); // the first led.
    if (i>0) {
        lightAllTraces(i-1, MEDIUMLEVEL); 
        if (i>1) {
           lightAllTraces(i-2, LOWLEVEL); 
           if(i>2 && JUMP >3) {
                lightAllTraces(i-3, 0); // turn off the distant led.            
           }
        }        
    }  
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
}

int LastLed (int i) {
  lightAllTraces(i, HIGHLEVEL); 
  lightAllTraces(i-1, MEDIUMLEVEL);
  lightAllTraces(i-2, LOWLEVEL); 
  if (JUMP > 3) {
    lightAllTraces(i-3, 0); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(delayval); // Delay for a period of time (in milliseconds).

  
  lightAllTraces(i, MEDIUMLEVEL);                     
  lightAllTraces(i-1,LOWLEVEL);
  if (JUMP > 3) {
   lightAllTraces(i-2, 0);                 
  }
  head_color = (head_color+1)%NUMOFCOLORS;                    
  lightAllTraces(i-JUMP+1,HIGHLEVEL); 


  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(delayval); // Delay for a period of time (in milliseconds).

  lightAllTraces(i-JUMP+2,HIGHLEVEL); 
  lightAllTraces(i-JUMP+1, MEDIUMLEVEL);                     
  head_color = (head_color-1)%NUMOFCOLORS;                    
  lightAllTraces(i, LOWLEVEL);
  if (JUMP > 3) {
   lightAllTraces(i-1, 0);                                 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(delayval); // Delay for a period of time (in milliseconds).
  if (JUMP > 3) { 
    lightAllTraces(i, 0);    
  }
  head_color = (head_color+1)%NUMOFCOLORS;                    
  lightAllTraces(i-JUMP+3, HIGHLEVEL); 
  lightAllTraces(i-JUMP+2, MEDIUMLEVEL);                     
  lightAllTraces(i-JUMP+1, LOWLEVEL);

  i=i-JUMP+3;
  return i;
}
void lightAllTraces(int i, float power) {
  int trace_num = 0;
   while (i >= 0 ) {     
     int color = (head_color+trace_num)%NUMOFCOLORS;
      pixels.setPixelColor(i, Wheel(color,power));
      i-=JUMP;     
      trace_num++;
      if (power == HIGHLEVEL) {
         power = LOWLEVEL;      
      } else if (power == LOWLEVEL) {
         power = HIGHLEVEL;      
      }
   } 
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// the level is how brigt will be tghe light (0 to 255).
uint32_t Wheel(byte color, float level) {

  if (level==0) return pixels.Color(0, 0, 0);
  //level=100;   
  float power;
 // Serial.println(color);
  if(color < NUMOFCOLORS/3) {
     power=1.0*color/(NUMOFCOLORS/3)*255;
     return pixels.Color(level*(255 - power), 0, level*power); 
  } else if(color < 2*NUMOFCOLORS/3) {
      color -= NUMOFCOLORS/3;
      power=1.0*color/(NUMOFCOLORS/3)*255;
      return pixels.Color(0, level*power, level*(255 - power));
  } else {
     color -= 2*NUMOFCOLORS/3;
     power=1.0*color/(NUMOFCOLORS/3)*255;
     return pixels.Color(level*power, level*(255 - power), 0);
  }
}

