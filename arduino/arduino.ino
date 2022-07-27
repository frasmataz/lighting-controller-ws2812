#include <FastLED.h>

#define LOGGING false
#define NUM_STRIPS 1
#define NUM_LEDS_PER_STRIP 72
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
float anim_time = 0.0;

float val = 150;            // 0   - 255    // "volume"
float hue = 190;            // 0   - 360    // "mood"
float sat = 255;            // 0   - 255    // "vibrance"
float strip_spread = 0.8;   // 0.0 - 2.0    // "divergence"
float twinkle_hue = 0.5;    // 0.0 - 1.0    // "contour"
float twinkle_sat = 0.2;    // 0.0 - 2.0    // "shimmer"
float twinkle_val = 2.0;    // 0.0 - 2.0    // "ember"
float speed = 0.5;          // 0.0 - 0.5    // "pace"

int volumePin = A0;
int contourPin = A1;
int divergencePin = A2;
int moodPin = A3;
int emberPin = A4;
int shimmerPin = A5;
int pacePin = A6;
int vibrancePin = A7;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, 9>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 10>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 11>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 12>(leds[3], NUM_LEDS_PER_STRIP);
}

void loop() {
  int frame_start = millis();
  readPots();
  // This outer loop will go over each strip, one at a time
  for(int x = 0; x < NUM_STRIPS; x++) {
    // This inner loop will go over each led in the current strip, one at a time
    for(int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      // Optimisation: hue and sat only calculated if val != 0, as LED would be off.
      int ledVal = getLedVal(x, i);
      int ledHue = ledVal == 0 ? 0 : getLedHue(x, i);
      int ledSat = ledVal == 0 ? 0 : getLedSat(x, i);
        
      leds[x][i] = CHSV(
        ledHue,
        ledSat,
        ledVal
      );

    }
  }

  FastLED.show();

  anim_time += speed * 10;
//  delay(1000);
  int frame_end = millis();
  if (LOGGING) {
    Serial.print("frame time: ");
    Serial.println(frame_end - frame_start);
  }
  
}

int getLedHue(int x, int i) {
  // First noise function varies the whole strip over time, second adds a colour wave throughout each strip
  return hue + ((inoise8(x * 110, anim_time) - 128) * strip_spread) + (twinkle_hue * (inoise8(x * 90, i * 30, anim_time) - 128));
}

int getLedSat(int x, int i) {
  float overdrive = 6.0f; // Scale noise amplitude, clamping values above max and below min.  Adds more 'shimmer' contrast.
  int noiseScale  = 120;   // Scale noise frequency.  Higher value means smaller waves.
  return constrain(                     // \/ Tweak this value adjust maximum selectable effect, default 255.
    sat * ( ( overdrive/2 - ((twinkle_sat/320.0f) * inoise8(x*noiseScale, i*noiseScale, anim_time*2) * overdrive) ) )
  , 0, 255);
}

int getLedVal(int x, int i) {
  float overdrive = 3.0f; // Scale noise amplitude, clamping values above max and below min.  Adds more 'ember' contrast.
  int noiseScale  = 60;   // Scale noise frequency.  Higher value means smaller waves.
  return constrain(                     // \/ Tweak this value adjust maximum selectable effect, default 255.
    val * ( ( overdrive/2 - ((twinkle_val/300.0f) * inoise8(x*noiseScale, i*noiseScale, anim_time) * overdrive) ) )
  , 0, 255);
}

void readPots() {  
  val =           (1023 - pow(analogRead(volumePin)/13.1,1.6))   / 1024.0f * 255.0f;
  hue =           (1023 - analogRead(moodPin))        / 1024.0f * 255.0f;
  //sat =           (1023 - analogRead(vibrancePin))    / 1024.0f * 255.0f;
  strip_spread =  (1023 - analogRead(divergencePin))  / 1024.0f * 1.5f;
  twinkle_hue =   (1023 - analogRead(contourPin))     / 1024.0f * 1.0f;
  twinkle_sat =   (1023 - analogRead(shimmerPin))     / 1024.0f * 1.5f;
  twinkle_val =   (1023 - analogRead(emberPin))       / 1024.0f * 2.0f;
  speed =         (1023 - analogRead(pacePin))        / 1024.0f * 0.5f;

  twinkle_sat = twinkle_sat > 0.1? twinkle_sat : 0.0;

  if (LOGGING) {
  //
//    Serial.print("val: ");
//    Serial.println(val);
//  //  Serial.print("hue: ");
//  //  Serial.println(hue);
//  //  Serial.print("sat: ");
//  //  Serial.println(sat);
//  //  Serial.print("strip_spread: ");
//  //  Serial.println(strip_spread);
//  //  Serial.print("twinkle_hue: ");
//  //  Serial.println(twinkle_hue);
//    Serial.print("twinkle_sat: ");
//    Serial.println(twinkle_sat);
//  //  Serial.print("twinkle_val: ");
//  //  Serial.println(twinkle_val);
//    Serial.print("speed: ");
//    Serial.println(speed);
  }
}
