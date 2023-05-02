/*
  Communications v3

  Transmits data using a white LED and recieves it using a photoresistor

*/


int ledState = LOW;             // ledState used to set the LED


// the setup routine runs once when you press reset:
void setup()
{
  // set the digital pin as output:
  pinMode(3, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}


const long txInterval = 150;              // interval at which to tx bit (milliseconds)
int tx_state = 0;
char chr;
unsigned long previousTxMillis = 0;        // will store last time LED was updated

char tx_string[] = "Pls i hav paper";
#define TX_START_OF_TEXT  -1
int tx_string_state = TX_START_OF_TEXT;

#define STX 0x70
#define ETX 0x71

char txButton, txTilt, txPot, txA, txB, txC, txD;
char txBuffer[8] = {0,0,0,0,0,0,0,ETX};
char rxBuffer[7];
char rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
int  rx_index;

void readInputs()
{
  // Reads the inputs in the mini-projects
  // Uses txButton, txTilt, txPot, txA, txB, txC, txD;

    txButton = digitalRead(2);
    txTilt = digitalRead(4);
    txPot = analogRead(A0) / 4;
    txA = digitalRead(5);
    txB = digitalRead(6);
    txC = digitalRead(7);
    txD = digitalRead(8);
}

void writeOutputs()
{
  // Writes the outputs in the mini-projects
  // Uses rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;

    digitalWrite(9, rxA);
    digitalWrite(10, rxB);
    digitalWrite(11, rxC);
    digitalWrite(12, rxD);

  
  
}

char getTxChar()
{
  char chr;
  
  switch (tx_string_state)
  {
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
    if (chr == ETX)  /* End of string? */
    {
      tx_string_state = TX_START_OF_TEXT;  /* Update the tx string state to start sending the string again */
      return ETX;  /* Send End of Text character */
    }
    else
    {
      return chr;  /* Send a character in the string */
    }
    break;
  }
}


void txChar()
{
  unsigned long currentTxMillis = millis();

  if (currentTxMillis - previousTxMillis >= txInterval)
  {
    // save the last time you blinked the LED (improved)
    previousTxMillis = previousTxMillis + txInterval;  // this version catches up with itself if a delay was introduced

    switch (tx_state)
    {
      case 0:
        chr = getTxChar();
        digitalWrite(3, HIGH);  /* Transmit Start bit */
        tx_state++;
        break;

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        if ((chr & 0x40) != 0)   /* Transmit each bit in turn */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        chr = chr << 1;  /* Shift left to present the next bit */
        tx_state++;
        break;

      case 8:
        digitalWrite(3, HIGH);  /* Transmit Stop bit */
        tx_state++;
        break;

      default:
        digitalWrite(3, LOW);
        tx_state++;
        if (tx_state > 20) tx_state = 0;  /* Start resending the character */
        break;
    }
  }
}



const long rxInterval = 15;              // interval at which to read bit (milliseconds)
int rx_state = 0;
char rx_char;
unsigned long previousRxMillis = 0;        // will store last time LED was updated
int rx_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void rxChar()
{
  unsigned long currentRxMillis = millis();
  int sensorValue;
  int i;

  if (currentRxMillis - previousRxMillis >= rxInterval)
  {
    // save the last time you read the analogue input (improved)
    previousRxMillis = previousRxMillis + rxInterval;  // this version catches up with itself if a delay was introduced

    sensorValue = analogRead(A0);
    //Serial.println(rx_state);

    switch (rx_state)
    {
      case 0:
        if (sensorValue >= 900)
        {
          rx_bits[0]++;
          rx_state++;
        }
        break;

      case 100:
        if ((rx_bits[0] >= 6) && (rx_bits[8] >= 6))  /* Valid start and stop bits */
        {
          rx_char = 0;

          for (i = 1; i < 8; i++)
          {
            rx_char = rx_char << 1;
            if (rx_bits[i] >= 6) rx_char = rx_char | 0x01;
          }
          i = (int)rx_char;
          Serial.println(rx_char);
        }
        else
        {
          Serial.println("Rx error");
        }
//        for (i = 0; i < 10; i++)  /* Print the recieved bit on the monitor - debug purposes */
//        {
//          Serial.println(rx_bits[i]);
//        }
        for (i = 0; i < 10; i++)
        {
          rx_bits[i] = 0;
        }
        rx_state = 0;
        break;

      default:
        if (sensorValue >= 900)
        {
          rx_bits[rx_state / 10]++;
        }
        rx_state++;
        break;
    }
  }

}



// the loop routine runs over and over again forever:
void loop()
{
  txChar();
  rxChar();
}

