#ifndef CONFIG_H
#define CONFIG_H
//GPRS details
// Your GPRS credentials, if any
const char apn[] = "YOURAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

// MQTT details
const char *broker = "mqtt.tingg.io";
const char *topicLocation = "location";
const int port = 1883;
const char* mqtt_device_id = "YOUR THING ID";//tingg thing_id
const char* mqtt_password = "YOUR THING KEY";//tingg thing_key
const char* mqtt_username = "thing";

//how often do we need to send messages
#define SEND_INTERVAL (300 * 1000)     

//uncomment if your GPS modul is connected via I2C or Serial2 - only choose one!
//#define GPS_USE_I2C
#define GPS_USE_SERIAL2

#define GPS_BAUD 9600
#define GPS_RX_PIN 18
#define GPS_TX_PIN 19

#endif
