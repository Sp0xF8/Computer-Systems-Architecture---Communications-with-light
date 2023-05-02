#include </Users/fate/Development/University/Computer Architecture/MiniProjects/Buzzer/notes.h>

const int buttonPin = 2; // the number of the pushbutton pin

int buttonState = 0; // variable for reading the pushbutton status

/*
0,
    NOTE_GS3, NOTE_A3, NOTE_E3, NOTE_A3, NOTE_A3, NOTE_E3, NOTE_A3 && NOTE_E3, NOTE_A3


    2,
    2, 2, 2, 4, 2, 2, 2, 2
*/

int melody[] = {

    NOTE_E4, NOTE_DS4, NOTE_E4, NOTE_DS4, NOTE_E4, NOTE_B3, NOTE_D4, NOTE_C4, NOTE_A3, 0,
    NOTE_D3, NOTE_F3, NOTE_A3, NOTE_B3, 0,
    NOTE_F3, NOTE_AS3, NOTE_B3, NOTE_C4, 0,
    NOTE_E4, NOTE_DS4, NOTE_E4, NOTE_DS4, NOTE_E4, NOTE_B3, NOTE_D4, NOTE_C4, NOTE_A3, 0,
    NOTE_D3, NOTE_F3, NOTE_A3, NOTE_B3, 0,
    NOTE_F3, NOTE_C4, NOTE_B3, NOTE_A3, 0,
    NOTE_B3, NOTE_C4, NOTE_D4, NOTE_E4, 0,
    NOTE_G3, NOTE_F4, NOTE_E3, NOTE_D4, 0,
    NOTE_E3, NOTE_E4, NOTE_D4, NOTE_C4, 0,
    NOTE_D3, NOTE_D4, NOTE_C4, NOTE_B3, 0,

};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
float noteDurations[] = {

    4, 4, 4, 4, 4, 4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,
    4, 4, 4, 4, 4, 4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,
    4, 4, 4, 1, 1.1,

};

void setup() {
    Serial.begin(9600);
    delay(5);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
}

void playTone(){
      for (int thisNote = 0; thisNote < 60; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.

    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

    int noteDuration = 1000 / noteDurations[thisNote];

    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.

    // the note's duration + 30% seems to work well:

    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);

    // stop the tone playing:

    noTone(8);

  }
}

void loop() {
    // read the state of the pushbutton value:
    buttonState = digitalRead(buttonPin);
    
        if (buttonState == HIGH) {
            Serial.println("Button is pressed");
            playTone();
            delay(100);
            
        } else {
            Serial.println("Button is not pressed");
        }

}