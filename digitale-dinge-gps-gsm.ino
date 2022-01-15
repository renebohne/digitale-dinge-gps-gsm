#include "config.h"
//#include "fakegps.h"
#include "sleep.h"
#include "gps.h"
#define SIM800L_IP5306_VERSION_20190610
#include "utilities.h"
#include "modem.h"


unsigned long start = 0;

// deep sleep support
RTC_DATA_ATTR int bootCount = 0;
esp_sleep_source_t wakeCause;  // the reason we booted this time

void setup()
{
  setupModem();
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Digitale Dinge GPS Demo"));

  initDeepSleep();


  int bl = getBatteryLevel();
  Serial.print("battery level: ");
  Serial.println(bl);

  if (!isCharging())
  {
    Serial.print("not ");
  }
  Serial.println("charging.");
  setupGPS();

  getGPS();

  modemBegin();
  mqttsetup();

  getGPS();

  if(!waitForGPSLock())
  {
    sleep();
  }

  
  digitalWrite(LED_GPIO, LED_ON);
  modemReconnect();
  mqttreconnect();
  if (gps.location.isValid())
  {
    String s = String(gps.location.lat()) + "," + String(gps.location.lng());
    mqtt.publish("location", s.c_str());
  }

  mqtt.publish("battery", String(getBatteryLevel()).c_str());
  digitalWrite(LED_GPIO, LED_OFF);
  delay(500);
  sleep();
}

void loop()
{

  //no loop because we use deep sleep in setup
}

void doDeepSleep(uint64_t msecToWake)
{
  Serial.printf("Entering deep sleep for %llu seconds\n", msecToWake / 1000);

  //esp_wifi_stop();

  //switch modem off
  digitalWrite(MODEM_POWER_ON, LOW);

  // FIXME - use an external 10k pulldown so we can leave the RTC peripherals powered off
  // until then we need the following lines
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

  // Only GPIOs which are have RTC functionality can be used in this bit map: 0,2,4,12-15,25-27,32-39.
  //uint64_t gpioMask = (1ULL << BUTTON_PIN);

  // FIXME change polarity so we can wake on ANY_HIGH instead - that would allow us to use all three buttons (instead of just the first)
  //gpio_pullup_en((gpio_num_t) BUTTON_PIN);

  //esp_sleep_enable_ext1_wakeup(gpioMask, ESP_EXT1_WAKEUP_ALL_LOW);

  esp_sleep_enable_timer_wakeup(msecToWake * 1000ULL);  // call expects usecs
  esp_deep_sleep_start();                               // TBD mA sleep current (battery)
}


void sleep() {
  // Set the user button to wake the board
  //sleep_interrupt(BUTTON_PIN, LOW);

  // We sleep for the interval between messages minus the current millis
  // this way we distribute the messages evenly every SEND_INTERVAL millis
  uint32_t sleep_for = (millis() < SEND_INTERVAL) ? SEND_INTERVAL - millis() : SEND_INTERVAL;
  doDeepSleep(sleep_for);
}

// Perform power on init that we do on each wake from deep sleep
void initDeepSleep() {
  bootCount++;
  wakeCause = esp_sleep_get_wakeup_cause();
  Serial.printf("booted, wake cause %d (boot count %d)\n", wakeCause, bootCount);
}

void getBateryValue() {
 int a = analogRead(35);
 
}

//wait for up to five seconds for GPS lock. If no lock within 5 seconds, return false.
bool waitForGPSLock()
{
  bool ledstate = false;
  for(int i=0; i<5; i++)
  {
    getGPS();
    if(gps.location.isValid())
    {
      return true;
    }
    ledstate = !ledstate;
    digitalWrite(LED_GPIO, ledstate);
    delay(1000);
  }
  return false;
}
