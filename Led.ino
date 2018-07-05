#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);   // set valtage HIGH to turn the LED on
  delay(1000);              // wait between writes
  digitalWrite(LED_PIN, LOW);    // set valtage LOW to turn the LED off
  delay(1000);              // wait between writes
}
