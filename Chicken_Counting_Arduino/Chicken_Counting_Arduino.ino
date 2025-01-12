/*
  Premium Chicken Farm
  Hayley Nguyen (gnguye5), gnguye5@uic.edu
  Rushil Patel (rpate476), rpate476@uic.edu
  Yug Patel (ypatel63), ypatel63@uic.edu
  This project develops an automated chicken care system with functionalities of counting chickens, water pumping, and playing music, all overseen by a central Arduino controller. 
  Multiple Arduinos allow efficient, independent task management: the Chicken Counting Arduino tracks movement and updates a display, the Water Pumping Arduino controls water flow based on sensors, and the Music Playing Arduino plays scheduled tunes. 
  The project has a timeline, aiming to create a scalable model for ethical, automated chicken farms.
*/


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// SoftwareSerial pins
SoftwareSerial waterPumpSerial(10, 11); // RX, TX
SoftwareSerial speakerSerial(8,9); // RX, TX

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

const int sensorPin = 2;           
int chickenCount = 0;              
int fenceIsOpen = 0;              
unsigned long fenceOpenTime = 0;   
const unsigned long fenceDelay = 1000; 
bool speakerSignalSent = false;          
bool waterSignalSent = false;           

void setup() {
    Serial.begin(9600); // Start Serial Monitor communication
    waterPumpSerial.begin(9600);         
    speakerSerial.begin(9600);         
    lcd.init();
    lcd.backlight();

    pinMode(sensorPin, INPUT);
    myservo.attach(4);             
    myservo.write(100); // The degrees of the servo motor that keeps the fence closed

    Serial.println("Chicken Counting System Initialized");
}

void loop() {
    // Detect chicken passing through
    if (digitalRead(sensorPin) == LOW && fenceIsOpen == 0) {
        Serial.println("Chicken detected!");

        // Open the fence
        fenceIsOpen = 1;
        myservo.write(0); 
        chickenCount++;            
        Serial.print("Chicken Count: ");
        Serial.println(chickenCount);

        fenceOpenTime = millis(); // Record time when fence was opened
        waterSignalSent = false;
        speakerSignalSent = false;
    }

    // Close the fence after delay
    if (fenceIsOpen == 1 && millis() - fenceOpenTime >= fenceDelay) {
        Serial.println("Closing the fence");
        myservo.write(100); 
        fenceIsOpen = 0;
    }

    // Send signal to start music when chicken count reaches 5
    if (chickenCount == 5 && !speakerSignalSent) {
        Serial.println("Sending START_MUSIC command");
        speakerSerial.println("START_MUSIC"); 
        speakerSignalSent = true;
    }

    // Send signal to activate the water pump when chicken count reaches 5
    if (chickenCount == 5 && !waterSignalSent) {
        Serial.println("Sending START_WATER command");
        waterPumpSerial.println("START_WATER"); 
        waterSignalSent = true;
        chickenCount = 0;        
    }

    // Update LCD display
    lcd.setCursor(0, 0);
    lcd.print("    WELCOME!    ");
    lcd.setCursor(0, 1);
    lcd.print("CHICKEN COUNT: ");
    lcd.print(chickenCount);

}