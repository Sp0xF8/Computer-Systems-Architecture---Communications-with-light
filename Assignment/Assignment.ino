

/*
  Communications v3

  Transmits data using a white LED and recieves it using a photoresistor

*/

#include <LiquidCrystal.h>;
//#include <SR04.h>;
#include <dht.h>;



int ledState = LOW;  // ledState used to set the LED
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//SR04 sr04 = SR04(9, 10); // initialize sensor that uses digital pins 9 and 10
dht DHT;



// the setup routine runs once when you press reset:
void setup() {

  lcd.begin(16, 2);


  // initialize pins as input


  pinMode(13, INPUT);  // TEMP / HUMIDITY

  pinMode(9, INPUT);   // SONAR 1
  pinMode(10, INPUT);  //SONAR 2


  // set the digital pin as output:



  pinMode(6, OUTPUT);  // LED - DATA TRANSFER

  pinMode(7, OUTPUT);  // LED



  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

const long txInterval = 200;  // interval at which to tx bit (milliseconds)
int tx_state = 0;
char chr;

unsigned long previousReadMillis = 0;  // will store last time inputes were read

unsigned long previousTxMillis = 0;  // will store last time LED was updated

#define TX_START_OF_TEXT -1
int tx_string_state = TX_START_OF_TEXT;

#define STX 0x70
#define ETX 0x71

char txButton, txTilt, txPot, txA, txB, txC, txD;
char txBuffer[8] = { 0, 0, 0, 0, 0, 0, 0, ETX };
char rxBuffer[7];
int rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
int rx_index;

bool buttonPressed = true;


void readInputs() {
  // reading inputs without blocking

  unsigned long currentTxMillis = millis();

  if (currentTxMillis - previousReadMillis >= 100) {  // if 60ms has passed since last tx
    previousReadMillis += 100;                        // increment previousTxMillis by 60ms

    // Serial.println("Reading Inputs"); // prints to serial monitor -- debug
    int chk = DHT.read11(13);

    if (analogRead(A3) > 1000) { // checks analog value of button
      buttonPressed = !buttonPressed; // invert bool value


      Serial.println("Button Pressed"); // prints to serial monitor -- debug
    }

    if (buttonPressed) {
      txButton = 1;
    } else {
      txButton = 0;
    }

    if (analogRead(A2) == (11)) {
      txTilt = 0;  // right way yp -- filted = false
    } else {
      txTilt = 1;  // upside down -- Tilted = true
    }


    txPot = map(analogRead(A4), 0, 500, 0, 10);  // finds 0-10 value for potentiometer
    txA = int(DHT.temperature);                  // set transmission value for temp
    txB = int(DHT.humidity);                     //set transmission value for humidity
    
    signed int joyStick = map(analogRead(A0), 510, 42, 0, 100); // flips values and finds range between 0-100

    if (joyStick < 5){ // value of 5 to remove noise
      txC = 0; // removes any negative values
    } else if (joyStick >= 100) { //catches any errors with mapping
      txC = 99; // max value of display
    } else {
      txC = joyStick;
    }
  }

  txD = 99;

  // if((x > 400 && x < 600) && (y < 300)) {
  //   Serial.println("Forward");
  // } else if ((x > 400 && x < 600) && (y > 700)) {
  //   Serial.println("Back");
  // } else if ((y > 400 && y < 600) && (x < 300)) {
  //   Serial.println("Left");
  // } else if ((y > 400 && y < 600) && (x > 700)) {
  //   Serial.println("Right");
  // } else {
  //   Serial.println("Still");
  // }
}

void writeOutputs() {
  // Writes the outputs in the mini-projects
  // Uses rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;

  digitalWrite(7, rxTilt);  // BLUE LED

  lcd.clear();  // clears straggler text
  lcd.setCursor(0, 0);

  lcd.print("Distance");
  lcd.setCursor(0, 1);
  lcd.print(rxD);
  lcd.setCursor(4, 1);
  lcd.print("cm");
}

char getTxChar() {
  char chr;

  switch (tx_string_state) {
    case TX_START_OF_TEXT:
      tx_string_state = 0;
      txBuffer[0] = txButton;
      txBuffer[1] = txTilt;
      txBuffer[2] = txPot;
      txBuffer[3] = txA;
      txBuffer[4] = txB;
      txBuffer[5] = txC;
      txBuffer[6] = txD;
      return STX;
      break;

    default:
      chr = txBuffer[tx_string_state];
      tx_string_state++;
      if (chr == ETX) { /* End of string? */

        tx_string_state = TX_START_OF_TEXT; /* Update the tx string state to start sending the string again */
        return ETX;                         /* Send End of Text character */
      } else {
        return chr; /* Send a character in the string */
      }
      break;
  }
}

void txChar() {
  unsigned long currentTxMillis = millis();

  if (currentTxMillis - previousTxMillis >= txInterval) {
    // save the last time you blinked the LED (improved)
    previousTxMillis = previousTxMillis + txInterval;  // this version catches up with itself if a delay was introduced

    switch (tx_state) {
      case 0:
        chr = getTxChar();
        //Serial.println(chr);
        digitalWrite(6, HIGH); /* Transmit Start bit */
        tx_state++;
        break;

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        if ((chr & 0x40) != 0) { /* Transmit each bit in turn */
          digitalWrite(6, HIGH);
        } else {
          digitalWrite(6, LOW);
        }
        chr = chr << 1; /* Shift left to present the next bit */
        tx_state++;
        break;

      case 8:
        digitalWrite(6, HIGH); /* Transmit Stop bit */
        tx_state++;
        break;

      default:
        digitalWrite(6, LOW);
        tx_state++;
        if (tx_state > 20)
          tx_state = 0; /* Start resending the character */
        break;
    }
  }
}

const long rxInterval = 20;  // interval at which to read bit (milliseconds)
int rx_state = 0;
char rx_char;
unsigned long previousRxMillis = 0;  // will store last time LED was updated
int rx_bits[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void rxChar() {
  unsigned long currentRxMillis = millis();
  int sensorValue;
  int i;

  if (currentRxMillis - previousRxMillis >= rxInterval) {
    // save the last time you read the analogue input (improved)
    previousRxMillis = previousRxMillis + rxInterval;  // this version catches up with itself if a delay was introduced

    sensorValue = analogRead(A5);

    // Serial.println(sensorValue);
    // Serial.println(rx_state);

    switch (rx_state) {
      case 0:
        if (sensorValue >= 900) {
          rx_bits[0]++;
          rx_state++;
        }
        break;

      case 100:
        if ((rx_bits[0] >= 6) && (rx_bits[8] >= 6)) { /* Valid start and stop bits */
          rx_char = 0;

          for (i = 1; i < 8; i++) {
            rx_char = rx_char << 1;
            if (rx_bits[i] >= 6)
              rx_char = rx_char | 0x01;
          }

          switch (rx_char) {
            case STX:
              rx_index = 0;
              break;

            case ETX:
              rxButton = rxBuffer[0];
              rxTilt = rxBuffer[1];
              rxPot = rxBuffer[2];
              rxA = rxBuffer[3];
              rxB = rxBuffer[4];
              rxC = rxBuffer[5];
              rxD = rxBuffer[6];



              rx_index = 0;
              break;

            default:
              rxBuffer[rx_index] = rx_char;
              rx_index++;
              break;
          }
          int num_recieved = rx_char;
          Serial.println(num_recieved); /* Print the recieved character on the monitor - debug purposes */
        } else {
          Serial.println("Rx error");
        }
        // for (i = 0; i < 10; i++) /* Print the recieved bit on the monitor - debug purposes */
        // {
        //   Serial.println(rx_bits[i]);
        // }
        for (i = 0; i < 10; i++) {
          rx_bits[i] = 0;
        }
        rx_state = 0;
        break;

      default:
        if (sensorValue >= 900) {
          rx_bits[rx_state / 10]++;
        }
        rx_state++;
        break;
    }
  }
}

// // the loop routine runs over and over again forever:
void loop() {

  txChar();
  rxChar();
  readInputs();
  writeOutputs();
}
