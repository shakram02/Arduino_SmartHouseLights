#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define RELAY_LINE1_PIN 8

// I'm using an edited version of the library, if you're using the normal version which 
// exists in arduino setup directory use <NewPing.h>
#include "NewPing.h"

// Ultrasonic sensor object
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

unsigned int distance = 0;	// Current distance of any object facing the ultrasonic sensor
unsigned int critical_distance_cm = 50;	// Cutoff distance at which the light will switch
bool state = 0;

void setup() {
	Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
	pinMode(RELAY_LINE1_PIN, OUTPUT);
	digitalWrite(RELAY_LINE1_PIN, HIGH);	// Turn the light off
}

void loop() {
	delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
	UpdateDistance();
	Serial.print("Ultrasonic: ");
	Serial.print(distance); // Send ping, get distance in cm and print result (0 = outside set distance range)
	Serial.println("cm");

	// If someone is near the door
	if (distance < critical_distance_cm)
	{
		while (distance < critical_distance_cm)
		{
			// Check if they moved away
			UpdateDistance();

			delay(5);	// Do nothing until the person moves away from the door
		}

		state = !state; // Change the state of the relay

		if (state)
		{
			Serial.println("Door Open!");
			digitalWrite(RELAY_LINE1_PIN, LOW);	// Turn the light on
		}
		else
		{
			Serial.println("Door Closed!");
			digitalWrite(RELAY_LINE1_PIN, HIGH);	// Turn the light off
		}
	}
}

// Updates the value of the Ultrasonic reading
void UpdateDistance()
{
	// Read 7 values from the ultrasonic and get the median value ( median filter )
	// Gets rid of noisy reading
	distance = sonar.convert_cm(sonar.ping_median(7));
	
	// The value 0 indicates that the ultrasonic sensor is reading nothing in front of it
	// Set this distance to max distance so the light doesn't switch unnecessarily
	if (distance == 0)
	{
		distance = MAX_DISTANCE;
	}
}