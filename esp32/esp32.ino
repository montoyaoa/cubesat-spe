#include "SparkFun_SinglePairEthernet.h"
// Adafruit LSM6DS
// Adafruit BusIO
// Adafruit Unified Sensor
#include <Adafruit_LSM6DSOX.h>

SinglePairEthernet adin1110;
Adafruit_LSM6DSOX imu;

const int MAX_MSG_SIZE = 200;

byte artemisMAC[6] = {0x00, 0xE0, 0x22, 0xFE, 0xDA, 0xC9};
byte esp32MAC[6] = {0x00, 0xE0, 0x22, 0xFE, 0xDA, 0xCA};

byte xmitPacket[MAX_MSG_SIZE];
byte recvPacket[MAX_MSG_SIZE];

static void rxCallback(byte * data, int dataLen, byte * senderMac)
{
  memcpy(recvPacket, data, dataLen);
}

void setup() {
  memset(recvBuffer, 0, sizeof(recvBuffer));
  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("CubeSat SPE Demo - Sensor Board simulator");
  /* Start up adin1110 */
  if (!adin1110.begin(esp32MAC)) 
  {
    Serial.print("Failed to connect to ADIN1110 MACPHY. Make sure board is connected and pins are defined for target.");
    while(1); //If we can't connect just stop here      
  }
  Serial.println("Connected to ADIN1110 MACPHY");

  /* Set up callback, to control what we do when data is recieved */
  adin1110.setRxCallback(rxCallback);

  /* Wait for link to be established */
  Serial.println("Device Configured, waiting for connection...");
  while (adin1110.getLinkStatus() != true);

  Wire.begin();
  
  while (imu.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("Waiting for IMU to initialize...");
    delay(1000); //Freeze
  }
}

void loop() {
  /* If we are still connected, send a message */
  if(adin1110.getLinkStatus())
  {      
      Serial.print("Sending:\t");
      Serial.println(); //This is ok since we know they are all null terminated strings
      
      adin1110.sendData((byte *)xmitPacket, sizeof(xmitPacket), destinationMAC);
  }
  else
  {
      Serial.println("Waiting for link to resume sending");
  }
}
