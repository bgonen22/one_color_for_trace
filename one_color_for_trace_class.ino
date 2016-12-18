//#include <ArduinoSTL.h>

#include <Adafruit_NeoPixel.h>
#include <StandardCplusplus.h>
#include <vector>
#include <iterator>

using namespace std;

// Which pin on the Arduino is connected to the NeoPixels?

#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      50

// How many colors do you want (more colors, more gradient)?
#define NUMOFCOLORS      6


// jump between two traces (tail trace to head trace)
#define JUMP 3

// traces pwer will be like wave or like teeth
#define FADE 0 //1 - traces power like a wave, 0 always first led max power

// how many pixels in each trace
#define NUM_OF_PIXELS_IN_TRACE 3

// delay between iterations
int delayval = 100; // delay in milisec

// light level 1=255
float HIGHLEVEL=40;
float LOWLEVEL=6;

const byte interruptPin = 2;
volatile boolean start = 0 ;
int color = 0;
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int head_color = 0;
uint32_t getColor(byte color, byte user_power);
class trace
{  
   byte num_of_pixels;
   boolean first_led_power; //1 - highest power, 0 lowest
   byte color;
   byte high_level;
   byte low_level;
   int start_index;
   byte power_jump;
   public:
   void advance(int s) { start_index = start_index + s;}
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
 //     Serial.print("level ");
//      Serial.println(level);
      pixels.setPixelColor(pos, getColor(color,level));    

      
    }    
   }
   void setColor(int c) {
    color = c;   
   }
   int getStartIndex() {
   // Serial.print("star: ");
   // Serial.println(start_index);
    return start_index;
   }
   int getNumOfPixels() {
    return num_of_pixels;
   }
   trace(int first,int num, int c,byte high, byte low){
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
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), start_trace, FALLING);
}
// ---------------------------
//              LOOP
// ---------------------------
void loop() {
  //runTrace();
 //  pingPong();
 
  start_with_button();  
}

// ---------------------------
//              start_with_button
// ---------------------------
void start_with_button() {
  trace *t;  
  vector<trace>::iterator it;  
  while (1) {
    //Serial.println(start);
    if (start) {        
        t=new trace(1,NUM_OF_PIXELS_IN_TRACE,color,HIGHLEVEL,LOWLEVEL);
        color = (color+1)% NUMOFCOLORS;   
        trace_vec.push_back(*t); 
        delete t;
        start = 0;
    }
    it = trace_vec.begin();
    if (trace_vec.size() > 0 && (it->getStartIndex() - it->getNumOfPixels() >= NUMPIXELS) ) {      
      it = trace_vec.erase(it);            
    } 
    advanceAll();
    clearAll();
    drawAll();       
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).   
    
  
  }
}
// ---------------------------
//              pingPong
// ---------------------------
void pingPong() {
  trace *t;    
  int color = 0;
  t=new trace(1,NUM_OF_PIXELS_IN_TRACE,color,HIGHLEVEL,LOWLEVEL); 
  int ad = 1;  
  while(1) {     
     if ((ad ==1) & (t->getStartIndex()==NUMPIXELS)) {
       ad = -1;
       t->setColor(color);
       color = (color+1)% NUMOFCOLORS;     
     } else if ((ad == -1) & (t->getStartIndex() - t->getNumOfPixels() +1 == 0)) {
      ad = 1;
      t->setColor(color);
      color = (color+1)% NUMOFCOLORS;     
     }
     t->advance(ad);     
     clearAll();
     t->draw();   
     pixels.show(); // This sends the updated pixel color to the hardware.
     delay(delayval/10); // Delay for a period of time (in milliseconds).   
       
  }
    
}
// ---------------------------
//              runTrace
// ---------------------------
void runTrace() {  
  trace *t;  
  int color = 0;
  t=new trace(1,NUM_OF_PIXELS_IN_TRACE,color,HIGHLEVEL,LOWLEVEL);     
  trace_vec.push_back(*t);
  delete t;
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
    //  Serial.println(first_led_power);
 //     Serial.println(color);
      t=new trace(first_led_power,NUM_OF_PIXELS_IN_TRACE,color,HIGHLEVEL,LOWLEVEL); 
      t->advance(1); 
      trace_vec.push_back(*t); 
      delete t;
    }
    it = trace_vec.begin();
    if (it->getStartIndex() - it->getNumOfPixels() >= NUMPIXELS ) {                 
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
  //Serial.println("drawAll");
  if (trace_vec.size() == 0) {
    return;  
  }
   for (vector<trace>::iterator it = trace_vec.begin() ; it != trace_vec.end(); ++it) {
      it->draw();             
        
    }
    //Serial.println("exit drawAll");
}

//----------------------------
//  advanceAll
//----------------------------
void advanceAll() {
  //Serial.println("advanceAll");
  if (trace_vec.size() == 0) {
    return;  
  }
    for (vector<trace>::iterator it = trace_vec.begin() ; it != trace_vec.end(); ++it) {
      it->advance(1);    
    }
    //Serial.println("exit advanceAll");
}
//----------------------------
//  clearAll
//----------------------------
void clearAll() {
  //Serial.println("clearAll");
  if (trace_vec.size() == 0) {
    return;  
  }
  for (int i = 0; i< NUMPIXELS; ++i) {
    pixels.setPixelColor(i,0);
  }
  //Serial.println("exit clearAll");

}

//----------------------------
//  getColor
//----------------------------
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// the level is how brigt will be tghe light (0 to 255).
uint32_t getColor(byte color, byte user_power) {
  if (user_power==0) return pixels.Color(0, 0, 0); 
  float power;
  float level;
 // Serial.println(color);
  level = (1.0*user_power)/100;
  if(color < NUMOFCOLORS/3) {
     power=1.0*color/(NUMOFCOLORS/3)*255;
     return pixels.Color(level*(255 - power), 0, level*power); 
  } else if(color < 2*NUMOFCOLORS/3) {
      color -= NUMOFCOLORS/3;
      power=1.0*color/(NUMOFCOLORS/3)*255;
  //    Serial.println(pixels.Color(0, level*power, level*(255 - power)));
      return pixels.Color(0, level*power, level*(255 - power));
  } else {
     color -= 2*NUMOFCOLORS/3;
     power=1.0*color/(NUMOFCOLORS/3)*255;
     return pixels.Color(level*power, level*(255 - power), 0);
  }
}

void start_trace() {
  start = 1;
}

