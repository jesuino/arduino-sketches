/*

Simple sketch to change the color of a double color LED using an old REED SWITCH

Once the magnet approaches the reed switch, it turns the ping REED_SWITCH low, which will make the detected boolean variable change its value, finally the pins LED1 and LED2 will have their output reversed

*/


int LED1 = 13;
int LED2 = 12;
int REED_SWITCH = 8;
bool detected = false;

void setup() {
  pinMode(REED_SWITCH, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(9600); 
}

void loop() {
  if(digitalRead(REED_SWITCH) == LOW) {
    detected = !detected;
  }
  if(detected) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);    
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
  }
 delay(1000);
}
