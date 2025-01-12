/*
  Premium Chicken Farm
  Hayley Nguyen (gnguye5), gnguye5@uic.edu
  Rushil Patel (rpate476), rpate476@uic.edu
  Yug Patel (ypatel63), ypatel63@uic.edu
  This project develops an automated chicken care system with functionalities of counting chickens, water pumping, and playing music, all overseen by a central Arduino controller. 
  Multiple Arduinos allow efficient, independent task management: the Chicken Counting Arduino tracks movement and updates a display, the Water Pumping Arduino controls water flow based on sensors, and the Music Playing Arduino plays scheduled tunes. 
  The project has a timeline, aiming to create a scalable model for ethical, automated chicken farms.
*/


#include <SoftwareSerial.h>

int waterLevel = 0;            
const int waterSensor = A0;   
const int motorPin = 8;       

// LED Pins
const int emptyLowLED = 3; // LED for "Empty" and "Low" levels
const int mediumLED = 4; // LED for "Medium" level
const int fullLED = 5; // LED for "Full" level

unsigned long previousMillis = 0; 
const unsigned long interval = 1000; 

bool startWaterPumping = false; 

// SoftwareSerial pins
SoftwareSerial mySerial(10, 11); //RX, TX

void setup() {
    Serial.begin(9600); // Begin Serial communication for debugging
    mySerial.begin(9600); //Begin SoftwareSerial communication

    // Set motor and LED pins as outputs
    pinMode(motorPin, OUTPUT);
    pinMode(emptyLowLED, OUTPUT);
    pinMode(mediumLED, OUTPUT);
    pinMode(fullLED, OUTPUT);

    // Ensure initial states
    digitalWrite(motorPin, LOW);  
    digitalWrite(emptyLowLED, LOW); 
    digitalWrite(mediumLED, LOW);
    digitalWrite(fullLED, LOW);

    Serial.println("Water Pumping System Initialized");
}

void loop() {
    // Check for incoming signal
    if (mySerial.available() > 0) {
        String received = mySerial.readStringUntil('\n'); // Read incoming commands
        received.trim();
        Serial.print("Signal Received: ");
        Serial.println(received);

        if (received == "START_WATER") {
            Serial.println("Start Water Pumping");
            startWaterPumping = true;
        }
    }

    // Execute water pumping if the flag is set
    if (startWaterPumping) {
        unsigned long currentMillis = millis(); 

        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis; 

            waterLevel = analogRead(waterSensor); 
            Serial.print("Water Level: ");
            Serial.println(waterLevel);

            // Control LEDs and motor based on water level
            if (waterLevel <= 100) {  
                Serial.println("Water Level Empty, Turning motor ON");
                digitalWrite(motorPin, HIGH);         
                digitalWrite(emptyLowLED, HIGH);
                digitalWrite(mediumLED, LOW);
                digitalWrite(fullLED, LOW);
            } else if (waterLevel > 100 && waterLevel <= 380) { 
                Serial.println("Water Level Low, Keeping motor ON");
                digitalWrite(motorPin, HIGH);         
                digitalWrite(emptyLowLED, HIGH);
                digitalWrite(mediumLED, LOW);
                digitalWrite(fullLED, LOW);
            } else if (waterLevel > 380 && waterLevel <= 500) { 
                Serial.println("Water Level Medium, Turning motor OFF");
                digitalWrite(motorPin, LOW);          
                digitalWrite(emptyLowLED, LOW);
                digitalWrite(mediumLED, HIGH);
                digitalWrite(fullLED, LOW);
            } else if (waterLevel > 500) {        
                Serial.println("Water Level Full, Turning motor OFF");
                digitalWrite(motorPin, LOW);         
                digitalWrite(emptyLowLED, LOW);
                digitalWrite(mediumLED, LOW);
                digitalWrite(fullLED, HIGH);
                startWaterPumping = false; 
                Serial.println("Water Pumping Completed");
            }
        }
    }
}
