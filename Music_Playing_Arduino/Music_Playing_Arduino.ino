/*
  Premium Chicken Farm
  Hayley Nguyen (gnguye5), gnguye5@uic.edu
  Rushil Patel (rpate476), rpate476@uic.edu
  Yug Patel (ypatel63), ypatel63@uic.edu
  This project develops an automated chicken care system with functionalities of counting chickens, water pumping, and playing music, all overseen by a central Arduino controller. 
  Multiple Arduinos allow efficient, independent task management: the Chicken Counting Arduino tracks movement and updates a display, the Water Pumping Arduino controls water flow based on sensors, and the Music Playing Arduino plays scheduled tunes. 
  The project has a timeline, aiming to create a scalable model for ethical, automated chicken farms.
*/


#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// SoftwareSerial pins
SoftwareSerial mySerial(8, 9); // RX, TX for communication

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int speakerPin = 7;

// Predefined tunes of Fur Elise song
#define NOTE_DS5 622
#define NOTE_D5 587
#define NOTE_C5 523
#define NOTE_B4 494
#define NOTE_A4 440
#define NOTE_GS4 415
#define NOTE_E4 330
#define NOTE_REST 0

int melody[] = {
    NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4,
    NOTE_A4, NOTE_C5, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_GS4, NOTE_B4, NOTE_C5,
    NOTE_E4, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4,
    NOTE_A4, NOTE_REST
};

int durations[] = {
    8, 8, 8, 8, 8, 8, 8, 8, 4,
    8, 8, 8, 8, 8, 8, 8, 8, 4,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 4,
    4, 2
};

const float speedFactor = 1.8;
const unsigned long playDuration = 20000; // Music duration
unsigned long startTime = 0;
unsigned long noteStartTime = 0;
bool musicPlaying = false;
int currentNoteIndex = 0;

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    lcd.begin(16, 2);

    lcd.print("Chickens Love:");
    lcd.setCursor(0, 1);
    lcd.print("Fur Elise");
    Serial.println("Speaker Arduino Initialized");
}

void loop() {
    // Check for signal from Chicken Counting Arduino
    if (mySerial.available() > 0) {
        String received = mySerial.readStringUntil('\n'); // Read incoming message
        received.trim(); // Remove any trailing whitespace or newline characters
        Serial.print("Signal Received: ");
        Serial.println(received);

        if (received == "START_MUSIC") {
            Serial.println("Music Start");
            startMusic();
        }
    }

    // Play music if the flag is set
    if (musicPlaying) {
        playFurElise(speakerPin);
        if (millis() - startTime >= playDuration) {
            musicPlaying = false; // Stop music after duration
            stopMusic();
        }
    }
}

void startMusic() {
    Serial.println("Starting Music...");
    musicPlaying = true;
    startTime = millis(); // Record start time
    noteStartTime = millis(); // Start timing for the first note
    currentNoteIndex = 0; // Reset to the first note
}

void playFurElise(int pin) {
    int melodyLength = sizeof(melody) / sizeof(melody[0]);

    if (currentNoteIndex < melodyLength) {
        int noteDuration = (1000 / durations[currentNoteIndex]) * speedFactor;

        // Check if the current note's duration has passed
        if (millis() - noteStartTime >= noteDuration) {
            // Move to the next note
            noteStartTime = millis();
            currentNoteIndex++;

            // Stop playing the previous note
            noTone(pin);

            // Play the next note if not a rest
            if (currentNoteIndex < melodyLength && melody[currentNoteIndex] != NOTE_REST) {
                tone(pin, melody[currentNoteIndex]);
            }
        }
    }
}

void stopMusic() {
    lcd.clear();
    lcd.print("Music Stopped");
    noTone(speakerPin);
    Serial.println("Music Stopped");
}
