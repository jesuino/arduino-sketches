/*
 * Constants
 */
// other consts
const int TOTAL_LEDS = 9;
// PINS
const int LED_PINS[TOTAL_LEDS] = {13, 12, 11, 10, 9, 8, 7, 6, 5};
const int MIC_PIN = A0;
const int PEAK_ADJUST_PIN = A1; 
const int SPEED_ADJUST_PIN = A2;
const int AUTO_MODE_PIN = 3;
const int SOUND_MODE_PIN = 2;
const int SET_MODE_PIN = 4;
// Animation constants
const char ANIM_SEPARATOR = ',';
const char ANIM_ENDING = ';';
//const String DEFAULT_ANIMATION = "110000000,001100000,000011000,000000110,000000001,000000110,000011000,001100000,110000000,000000000,111111111,000000000,111111111;";
const String DEFAULT_ANIMATION = "100000000,001000000,000010000,000010000,000000100,000000001,000000010,000001000,000100000,010000000,000000000,111111111;";

/*
 * Globals
 */
int ledQueue[TOTAL_LEDS];

// variables for sound input
int mx = 0; 
int val = 0;
int maxValue = 1024;
bool automatic = true;
// variables for automatic playing
int animationStep = 0;
int totalSteps = 0;
String animationInfo = DEFAULT_ANIMATION;

void setup() {
  pinMode(MIC_PIN, INPUT);
  pinMode(SET_MODE_PIN, INPUT);
  pinMode(PEAK_ADJUST_PIN, INPUT);
  pinMode(SPEED_ADJUST_PIN, INPUT);
  pinMode(SOUND_MODE_PIN, OUTPUT);
  pinMode(AUTO_MODE_PIN, OUTPUT);
  for(int i = 0; i < TOTAL_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  } 
  Serial.begin(9600);
}

void loop() {
  checkAndSetMode(); 
  cleanQueue();
  if(automatic) {
    // TODO: read from serial to see if there's any animation, parse and set as the current animation
//    currentAnimation = DEFAULT_ANIMATION;
      loadAnimation(animationInfo);
  } else {
    collectAudioValues();
    setLedQueue();  
  }
  updateLeds();
  sendValues();
}

void checkAndSetMode() {
  int mode = digitalRead(SET_MODE_PIN);
  bool changedMode = false;
  if(mode == HIGH) {
    automatic =  !automatic;
    changedMode = true;
    delay(300);
  }
  if(automatic) {
    digitalWrite(AUTO_MODE_PIN , HIGH);
    digitalWrite(SOUND_MODE_PIN , LOW);
  } else {
    digitalWrite(SOUND_MODE_PIN , HIGH);
    digitalWrite(AUTO_MODE_PIN , LOW);
  }
  // if we just changed to automatic, we must read the serial input to load a new animation
  if( automatic && changedMode) {
    // TODO: 
    digitalWrite(AUTO_MODE_PIN , HIGH);
    digitalWrite(SOUND_MODE_PIN , HIGH);
    delay(10000);
  }
}

void loadAnimation(String animationStr) {
  int i = 0;
  int j = 0;
  char currentChar;
  do {
    if(i == animationStep) break;
    currentChar = animationStr.charAt(j);
    if(currentChar == ANIM_SEPARATOR) {
      i++;
    }   
    j++; 
  } while(currentChar != ANIM_ENDING);
   
  if(animationStep > i) { 
    animationStep = 0;
  } else {
    animationStep++;
  }
  for(i = 0; i < TOTAL_LEDS; i++) {
    if(animationStr.charAt(j + i) == '1') {
      ledQueue[i] = HIGH;
    }
  }
}

/*
  Clean the current queue of LEDs to be displayed
*/
void cleanQueue() {
  for(int i = 0; i < TOTAL_LEDS; i++) {
    ledQueue[i] = LOW;
   }
}

/*
  Configure the current led queue based on the audio input
*/
void setLedQueue() {
  int v = constrain(mx, 0, maxValue);
  int highLeds = map(v, 0, maxValue, 0, TOTAL_LEDS);
  for(int i = 0; i < highLeds; i++) { 
    ledQueue[i] = HIGH;
  }
}

/*
Collect the values from the eletrect mic
*/
void collectAudioValues() {
  // this source was took from https://arduinodiy.wordpress.com/2012/12/20/electret-microphone-amplifier/
  mx = 0;        // mx only increases
  // Perform 10000 reads. Update mn and mx for each one.
  for (int i = 0; i < 100; ++i) {
    val = analogRead(MIC_PIN);
    mx = max(mx, val);
  }
  // end
  maxValue = analogRead(PEAK_ADJUST_PIN);
}

/*
  Update the leds PIN based on the led queue. When automatic I can configure the delay.
*/
void updateLeds() {
   for(int i = 0; i < TOTAL_LEDS; i++) {
    digitalWrite(LED_PINS[i], ledQueue[i]);
   }  
   if(automatic) {
    int animationDelay = analogRead(SPEED_ADJUST_PIN) / 4;
    delay(animationDelay);
   }
 }
 
void sendValues() {
  Serial.print("M=");
  Serial.print(mx);  
  Serial.print(";maxValue=");
  Serial.print(maxValue);  
  Serial.print(";automatic=");
  Serial.print(automatic);  
  Serial.print(";totalLeds=");
  Serial.print(TOTAL_LEDS);
  Serial.println();
}
