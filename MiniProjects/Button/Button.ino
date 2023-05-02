const int buttonPin = 2; // the number of the pushbutton pin

int buttonState = 0; // variable for reading the pushbutton status

void setup() {
    Serial.begin(9600);
    delay(5);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
}

void loop() {
    // read the state of the pushbutton value:
    buttonState = digitalRead(buttonPin);
    
        if (buttonState == HIGH) {
            Serial.println("Button is pressed");
        } else {
            Serial.println("Button is not pressed");
        }

}