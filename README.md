# Arduino Automation -- Arduino Library
A dandy little library used to connect Arduinos to my Arduino Automation service.


### Required Libraries
- ArduinoJson
    - Used for deseralizing JSON from an Arduino Automation service.
- ArduinoHttpClient
    - Used for communicating between an arduino and an Arduino Automation serivce.
- ArduinoOTA
    - Please read the instructions in the ArduinoOTA library before using.
    - Used for OTA updates

### Known Limitations
- Requires an Arduino Automation service that is working.
- Currently only works for "in-home" automation, meaning that an internet connection between the Arduino and Arduino Automation service must be active.
    - IP Address must be reported to Arduino Automation in order for this to work.
- This library currently only supports and has been tested on an Arduino Nano 33 IOT and an ESP32 dev kit. Adding more devices isn't a challenge, but testing them would be.
