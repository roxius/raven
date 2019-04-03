#include <NewPing.h>

#define SONAR_NUM 2      // Number of sensors.
#define MAX_DISTANCE 400 // Maximum distance (in cm) to ping.

NewPing sonar[SONAR_NUM] = {   // Sensor object array.
  NewPing(13, 12, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping. 
  NewPing(11, 10, MAX_DISTANCE), 
};

void setup() {
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
}

void loop() { 
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through each sensor and display results.
    delay(1160); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    Serial.print(i);
    Serial.print("=");
    Serial.print(sonar[i].ping_cm());
    Serial.println("cm ");
  }
}
