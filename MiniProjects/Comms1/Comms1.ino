/*
  Communications

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


const long txInterval = 200;              // interval at which to tx bit (milliseconds)
int tx_state = 0;

unsigned long previousTxMillis = 0;        // will store last time LED was updated

void txChar(char chr)
{
  unsigned long currentTxMillis = millis();

  if (currentTxMillis - previousTxMillis >= txInterval)
  {
    // save the last time you blinked the LED
    previousTxMillis = currentTxMillis;

    switch (tx_state)
    {
      case 0:
        digitalWrite(3, HIGH);  /* Transmit Start bit */
        tx_state++;
        break;

      case 1:
        if ((chr & 0x40) != 0)   /* Transmit Bit 7 */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        tx_state++;
        break;

      case 2:
        if ((chr & 0x20) != 0)   /* Transmit Bit 6 */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        tx_state++;
        break;

      case 3:
        if ((chr & 0x10) != 0)   /* Transmit Bit 5 */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        tx_state++;
        break;

      case 4:
        if ((chr & 0x08) != 0)   /* Transmit Bit 4 */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        tx_state++;
        break;

      case 5:
        if ((chr & 0x04) != 0)   /* Transmit Bit 3 */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        tx_state++;
        break;

      case 6:
        if ((chr & 0x02) != 0)   /* Transmit Bit 2 */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        tx_state++;
        break;

      case 7:
        if ((chr & 0x01) != 0)   /* Transmit Bit 1 */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
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



const long rxInterval = 20;              // interval at which to read bit (milliseconds)
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
    // save the last time you read the analogue input
    previousRxMillis = previousRxMillis + rxInterval;

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
        if ((rx_bits[0] >= 3) && (rx_bits[8] >= 3))  /* Valid start and stop bits */
        {
          rx_char = 0;

          for (i = 1; i < 8; i++)
          {
            rx_char = rx_char << 1;
            if (rx_bits[i] >= 3) rx_char = rx_char | 0x01;
          }
          i = (int)rx_char;
          Serial.println(rx_char);
        }
        else
        {
          Serial.println("Rx error");
        }
        for (i = 0; i < 10; i++)
        {
          Serial.println(rx_bits[i]);
        }
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
  txChar('L');
  rxChar();
}