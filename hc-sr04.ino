#define echoPin 12 //echo pin
#define trigPin 13 //trigger pin
long duration, distance;
void setup() {

  Serial.begin(9600); // serial speed in boud
  pinMode(echoPin, INPUT); //set pin as input
  pinMode(trigPin, OUTPUT); //set pin as output
  
}

void loop() {

 digitalWrite(trigPin, LOW); //sets trig pin low for 5 seconds
 delayMicroseconds(5); 

 digitalWrite(trigPin, HIGH); // sets trig pin to high
 delayMicroseconds(10); //waits 10 microseconds to allow sensor transmit 8 sonic pulses
 
 digitalWrite(trigPin, LOW); // sets trig pin to low
 duration = pulseIn(echoPin, HIGH); // read duration in milliseconds
 
 distance = duration/58; // duration in milliseconds divided by the vaue given in datasheet
 Serial.print(distance);
 Serial.println(" cm");
 delay(1000);
 
}
