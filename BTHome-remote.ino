/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux
   Modified to work with NimBLE
   Modified for V2 by Chreece
   Modified by countrysideboy: Code cleanups, Chop Data, Encryption
   BLE advertisement format from https://bthome.io/

*/

#include "BTHome.h"
#include <AceButton.h>

#define DEVICE_NAME "BTHome-Remote"  // The name of the sensor
//#define ENABLE_ENCRYPT // Remove this line for no encryption
String BIND_KEY = "231d39c1d7cc1ab1aee224cd096db932"; // Change this key with a string containing 32 of: a-f and 0-9 characters (hex) this will be asked in HA
BTHome bthome;
uint8_t seq = 0;

using namespace ace_button;
#define ID_BATTERY		0x01
#define BTN_POWER      (0)
#define BTN_BACK       (2)
#define BTN_HOME       (4)
#define BTN_PLAY_PAUSE (5)
#define BTN_UP         (18)
#define BTN_LEFT       (19)
#define BTN_SELECT     (22)
#define BTN_RIGHT      (23)
#define BTN_DOWN       (25)
#define BTN_VOL_UP     (12)
#define BTN_MUTE       (13)
#define BTN_CH_UP      (14)
#define BTN_VOL_DOWN   (15)
#define BTN_SETTINGS   (16)
#define BTN_CH_DOWN    (17)
#define BTN_SHORTCUT_3 (32)
#define BTN_SHORTCUT_4 (33)
#define BTN_BRIGHT_DN  (26)
#define BTN_BRIGHT_UP  (27)
#define BTN_SHORTCUT_1 (34)
#define BTN_SHORTCUT_2 (35)
#define BATTERY        (36)

struct Info {
  const uint8_t buttonPin;
  const uint8_t mode;
};

Info INFOS[] = {
  { BTN_POWER, INPUT_PULLUP },
  { BTN_BACK, INPUT_PULLUP },
  { BTN_HOME, INPUT_PULLUP },
  { BTN_PLAY_PAUSE, INPUT_PULLUP },
  { BTN_UP, INPUT_PULLUP },
  { BTN_LEFT, INPUT_PULLUP },
  { BTN_SELECT, INPUT_PULLUP },
  { BTN_RIGHT, INPUT_PULLUP },
  { BTN_DOWN, INPUT_PULLUP },
  { BTN_VOL_UP, INPUT_PULLUP },
  { BTN_MUTE, INPUT_PULLUP },
  { BTN_CH_UP, INPUT_PULLUP },
  { BTN_VOL_DOWN, INPUT_PULLUP },
  { BTN_SETTINGS, INPUT_PULLUP },
  { BTN_CH_DOWN, INPUT_PULLUP },
  { BTN_SHORTCUT_3, INPUT_PULLUP },
  { BTN_SHORTCUT_4, INPUT_PULLUP },
  { BTN_BRIGHT_DN, INPUT_PULLUP },
  { BTN_BRIGHT_UP, INPUT_PULLUP },
  { BTN_SHORTCUT_1, INPUT },
  { BTN_SHORTCUT_2, INPUT },
};

AceButton buttons[sizeof(INFOS)/sizeof(Info)]; 

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState);


void setup() {
  Serial.begin(115200);
#ifdef ENABLE_ENCRYPT
  bthome.begin(DEVICE_NAME, true, BIND_KEY, false);
#else
  bthome.begin(DEVICE_NAME, false, "", false);
#endif                                                              
  pinMode( BATTERY, INPUT);

  for (uint16_t i = 0; i < sizeof(INFOS)/sizeof(Info); ++i)
  {
    pinMode(INFOS[i].buttonPin, INFOS[i].mode);
    buttons[i].init(INFOS[i].buttonPin, HIGH, i );
  }
    // Configure the ButtonConfig with the event handler, and enable all higher
  // level events.
  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
  bthome.start();

  Serial.println(F("setup(): ready"));
}

void loop() {
  // Should be called every 4-5ms or faster, for the default debouncing time
  // of ~20ms.
  for (uint8_t i = 0; i < sizeof(INFOS)/sizeof(Info); i++) {
    buttons[i].check();
  }
}
 
void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {

  // Get the LED pin
  uint8_t id = button->getId();

  // Print out a message for all events.
  Serial.print(F("handleEvent(): id: "));
  Serial.print(id);
  Serial.print(F(".   eventType: "));
  Serial.print(AceButton::eventName(eventType));
  Serial.println();


  uint16_t btHomeId = EVENT_BUTTON_NONE;
  switch (eventType) {
    case AceButton::kEventPressed:
      return;
    case AceButton::kEventReleased:
      return;
    case AceButton::kEventClicked:
      btHomeId = EVENT_BUTTON_PRESS;
      break;
    case AceButton::kEventDoubleClicked:
      btHomeId = EVENT_BUTTON_DOUBLE_PRESS;
      break;
    case AceButton::kEventLongPressed:
      btHomeId = EVENT_BUTTON_LONG_PRESS;
      break;
    case AceButton::kEventRepeatPressed:
      return;
    case AceButton::kEventLongReleased:
      return;
    case AceButton::kEventHeartBeat:
      return;
  }
  uint8_t raw[3] = {0,0,0};
  raw[0] = id;
  raw[1] = btHomeId;
  raw[2] = seq++;
  bthome.addMeasurement(ID_RAW, raw, sizeof(raw));

  bthome.sendPacket(0);

  //bthome.addMeasurement_state(ID_BATTERY, 50);
  //bthome.sendPacket(0);

  //bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_NONE);
  //bthome.sendPacket(0);

  //Serial.println("Sending message");

  //bthome.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_NONE);
  //bthome.sendPacket(0);
}