/*
Original Author: Eric Tsai
Updated by Ingo Haschler
License: CC-BY-SA, https://creativecommons.org/licenses/by-sa/2.0/
Date: 9-1-2014
Original File: UberSensor.ino
This sketch is for a wired Arduino w/ RFM69 wireless transceiver
Sends sensor data (gas/smoke, flame, PIR, noise, temp/humidity) back
to gateway.  See OpenHAB configuration file.
*/

#include <RFM69.h>  // from https://github.com/LowPowerLab/RFM69
#include <SPI.h>

#include <OneWire.h> // for Tempsensor
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 6
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);
//
// Configuration section 
// 1) Update encryption string "ENCRYPTKEY" and network id, node id and frequency
//

//
// Radio
//

#define NODEID        10    //unique for each node on same network
#define NETWORKID     27  //the same on all nodes that talk to each other
#define GATEWAYID     1	// always=1
//Match frequency to the hardware version of the radio (uncomment one):
//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY   RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "abcdefghijklmnop" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack

//
// Arduino
// Arduino has LED on D3
//
#define LED           3
#define SERIAL_BAUD   9600  //must be 9600 for GPS, use whatever if no GPS

#define DEVICE_TEMP 6
#define DEVICE_HUM 7
#define DHTPIN 7     			// digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

/*
 * General
 */
bool debug = 1;
#define SEND_INTERVAL 100 // time between transmissions (in ms)

#define KNOCK_ID 1
#define OPTIC_ID 2
#define BUTTON_ID 3
#define TEMP_ID 4

#define OPTIC_PIN 3
#define BUTTON_PIN 4
#define KNOCK_ISR_PIN 5



volatile byte knockCounter = 0;

/*
 * End configuration section
 */

//struct for wireless data transmission
typedef struct {
    short int       nodeID; 		//node ID (1xx, 2xx, 3xx);  1xx = basement, 2xx = main floor, 3xx = outside
    short int       deviceID;		//sensor ID (2, 3, 4, 5)
    unsigned long   uptime_ms; 		//uptime in ms
    float     sensordata;   	//sensor data?
    float     battery_volts;		//battery condition?
} Payload;

/*
 * RFM69 Pinout to arduino:
 *   MOSI = 11
 *   MISO = 12
 *   SCK = 13
 *   SS = 10
 *   DIO0 = D2
 */
RFM69 radio;

//
// send a message via RFM radio
//
void sendMessage(int deviceid, float sensordata)
{
    Payload msg;
    msg.nodeID = NODEID;
    msg.deviceID = deviceid;
    msg.uptime_ms = millis();
    msg.sensordata = sensordata;
	// TBD batteryVolts is a stub...
    msg.battery_volts = batteryVoltage();
    radio.send(GATEWAYID, (const void*)(&msg), sizeof(msg));
    blinkNTimes(1);
    Serial.println(sensordata);
	  radio.sleep();
}

//#include "DHT.h"
// Initialize DHT sensor for 8mhz Arduino
// DHT dht(DHTPIN, DHTTYPE, 2);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

//
// Handles DHT sensor
//
void sensorTempHum() {
	// fill in your code here...
}

//
// Blinks the led n times and waits 900ms
//
void blinkNTimes(int n)
{
  for(int i = 0; i < n; i += 1)
  {
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);              // wait for 1/10 second
    digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
    delay(100);             // wait for 1/10 second
  }
  delay(900);
}


//
// Print to serial in debug mode only
//
void debugPrintln(const char* s)
{
    if (debug)
    {
        Serial.println(s);
    }
}

void debugPrintf(const char* s, double x)
{
    if (debug)
    {
        char buff[256];
        sprintf(buff, s, x);
        Serial.println(buff);
    }
}

//
// Reads and calculates current battery voltage
// Returns: Current voltage in Volts.
//
float batteryVoltage()
{
	// TBD put your code here...
    return 0.0;
}

void knockSignal()
{
  if (knockCounter == 100)
  { 
    knockCounter = 0;
  }
  else
  {
    knockCounter++;
  }
}

void setup()
{
    delay(1000);
    if (debug)
        Serial.begin(SERIAL_BAUD);          //  setup serial
    
    while(!Serial);

    pinMode(KNOCK_ISR_PIN,INPUT_PULLUP);
    
    attachInterrupt((KNOCK_ISR_PIN), knockSignal, CHANGE);
    
    pinMode(LED, OUTPUT);
    pinMode(OPTIC_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT);
    sensors.begin();

    // first signal: started :-)
    blinkNTimes(1);

    if (!radio.initialize(FREQUENCY, NODEID, NETWORKID))
    {
      while(true)
      {
        blinkNTimes(5);
      }
    }
#ifdef IS_RFM69HW
    radio.setHighPower(); //uncomment only for RFM69HW!
#endif
    radio.encrypt(ENCRYPTKEY);
    radio.setPowerLevel(31);
    radio.sleep();

    if (debug)
    {
        char buff[50];
        sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
        Serial.println(buff);
    }
    // second signal: radion initialized
    blinkNTimes(2);

    //temperature / humidity sensor
    //dht.begin();
    debugPrintln("DHT sensor enabled.");
    // third signal: DHT initialized
    blinkNTimes(3);
    // send initial reading
    sensorTempHum();
}

void loop()
{
  //sensorTempHum();
  int opticVal = digitalRead(OPTIC_PIN); 
  int buttonVal = digitalRead(BUTTON_PIN);

  sensors.requestTemperatures();
  
  sendMessage(42, 42.0);
  delay(SEND_INTERVAL);
  sendMessage(KNOCK_ID, knockCounter);
  delay(SEND_INTERVAL);
  sendMessage(OPTIC_ID, opticVal);
  delay(SEND_INTERVAL);
  sendMessage(BUTTON_ID, buttonVal);
  delay(SEND_INTERVAL);
  sendMessage(TEMP_ID, sensors.getTempCByIndex(0));
  delay(SEND_INTERVAL);
}//end loop







