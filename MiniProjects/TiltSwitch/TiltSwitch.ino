/* Tilt Sensor
 * -----------
 *
 * Detects if the sensor has been tilted or not and
 * lights up the LED if so. Note that due to the
 * use of active low inputs (through a pull-up resistor)
 * the input is at low when the sensor is active.
 *
 * (cleft) David Cuartielles for DojoCorp and K3
 * @author: D. Cuartielles
 *
 */

int ledPin = 13;
int inPin = 7; 
int value = 0;

void setup() 
{
  pinMode(ledPin, OUTPUT);              // initializes digital pin 13 as output
  pinMode(inPin, INPUT);                // initializes digital pin 7 as input
}

void loop() 
{
  value = digitalRead(inPin);   // reads the value at a digital input 
  digitalWrite(ledPin, value);           
}