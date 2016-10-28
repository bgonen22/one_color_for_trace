#include <Adafruit_NeoPixel.h>
#include <StandardCplusplus.h>
#include <vector>
#include <iterator>
using namespace std;



// Which pin on the Arduino is connected to the NeoPixels?

#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60

// How many colors do you want (more colors, more gradient)?
#define NUMOFCOLORS      15


// jump between to traces (head trace to head trace - min JUMP = 3)
#define JUMP 3

// traces pwer will be like wave or like teeth
#define FADE 1 //1 - traces power like a wave, 0 always first led max power

// how many pixels in each trace
#define NUM_OF_PIXELS_IN_TRACE 3

// delay between iterations
int delayval = 100; // delay in milisec

// light level 1=255
float HIGHLEVEL=0.4;
float LOWLEVEL=0.06;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int head_color = 0;
uint32_t getColor(byte color, float level);
class trace
{  
   int num_of_pixels;
   int first_led_power; //1 - highest power, 0 lowest
   byte color;
   float high_level;
   float low_level;
   int start_index;
   float power_jump;
   public:
   void advance() { start_index++;}
   //---------
   // draw
   //---------
   virtual void draw() {
//    Serial.print("start: "); 
    //Serial.println(start_index); 
    for (int i = 0; i < num_of_pixels;++i) {   
      int pos = start_index - i;
      if (pos<0 || pos> NUMPIXELS) {
        continue;      
      }
      float level;
      if (first_led_power) {
        level = high_level - i*power_jump;
      } else {
        level = low_level + i*power_jump;        
      }
      
      pixels.setPixelColor(pos, getColor(color,level));    

      
    }    
   }
   int getStartIndex() {
   // Serial.print("star: ");
   // Serial.println(start_index);
    return start_index;
   }
   int getNumOfPixels() {
    return num_of_pixels;
   }
   trace(int first,int num, int c,float high, float low){
    first_led_power = first;
    num_of_pixels = num;
    color = c;
    high_level = high;
    low_level  = low;
    start_index = -1;
    power_jump = (high_level-low_level)/num_of_pixels;
   }
};

vector<trace> trace_vec;
void setup() {
 Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.
  //Serial.println("setup");
  
}
// ---------------------------
//              LOOP
// ---------------------------
void loop() {  
  trace *t;  
  int color;
  t=new trace(1,NUM_OF_PIXELS_IN_TRACE,color,HIGHLEVEL,LOWLEVEL);     
  trace_vec.push_back(*t);
  vector<trace>::iterator it;  
  //Serial.println("begin ");
  int first_led_power = 1;
  while (1) {
    advanceAll();
    it = trace_vec.end();
    it--;              
    if (it->getStartIndex() - it->getNumOfPixels() - JUMP == 0) {
      color = (color + 1)% NUMOFCOLORS;
      if (FADE) {
        first_led_power = !first_led_power;
      }
      Serial.println(first_led_power);
      
      t=new trace(first_led_power,NUM_OF_PIXELS_IN_TRACE,color,HIGHLEVEL,LOWLEVEL); 
      t->advance(); 
      trace_vec.push_back(*t); 
      delete t;
    }
    it = trace_vec.begin();
    if (it->getStartIndex() - it->getNumOfPixels() >= NUMPIXELS ) {      
      //Serial.println("del");      
      //delete it;      
      it = trace_vec.erase(it);      
    }
    clearAll();
    drawAll();   
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).   
    
  }

}

//----------------------------
//  drawAll
//----------------------------
void drawAll() {
   for (vector<trace>::iterator it = trace_vec.begin() ; it != trace_vec.end(); ++it) {
      it->draw();             
        
    }
}

//----------------------------
//  advanceAll
//----------------------------
void advanceAll() {
    for (vector<trace>::iterator it = trace_vec.begin() ; it != trace_vec.end(); ++it) {
      it->advance();    
    }
}
//----------------------------
//  clearAll
//----------------------------
void clearAll() {
  for (int i = 0; i< NUMPIXELS; ++i) {
    pixels.setPixelColor(i,0);
  }

}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// the level is how brigt will be tghe light (0 to 255).
uint32_t getColor(byte color, float level) {
  if (level==0) return pixels.Color(0, 0, 0); 
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

