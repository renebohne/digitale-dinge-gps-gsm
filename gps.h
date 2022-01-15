#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <Arduino.h>
#include "config.h"

#ifdef GPS_USE_I2C
#include <Wire.h>
#endif

TinyGPSPlus gps;

void displayInfo()
{
  Serial.println();
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

#ifdef GPS_USE_I2C
void setupGPS() {
  byte error;
  int nDevices = 0;
  byte sensorAddr[] = {0x42, 0x40};
  Serial.println(F("\nI2C Scanning..."));
  for (int i = 0; i < sizeof(sensorAddr); i++) {
    Wire.beginTransmission(sensorAddr[i]);
    error = Wire.endTransmission();
    if (error == 0) {
      nDevices++;
      switch (sensorAddr[i])
      {
        case 0x42:
          Serial.println(F("CAM-M8Q GPS found."));
          break;
      }
    }
  }
  if (nDevices == 0) {
    Serial.println(F("No I2C devices found.\nCheck cable connections and press Reset."));
    while (true);
  } else {
    Serial.print(nDevices);
    Serial.println(F(" sensors found.\n"));
  }
}


void getGPS()
{
  Wire.requestFrom(0x42, 10);
  while (Wire.available()) {
    char c = Wire.read();

    if (c != 0xFF)
    {
      Serial.print(c);
    }
    if (gps.encode(c)) {
      displayInfo();
    }
  }
}
#endif

#ifdef GPS_USE_SERIAL2
void setupGPS()
{
  Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    
}

void getGPS()
{
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c != 0xFF)
    {
      Serial.print(c);
    }
    if (gps.encode(c)) {
      displayInfo();
    }
  }
}
#endif

#endif
