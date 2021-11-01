#ifndef aa_lib_h
#define aa_lib_h

#if defined(ESP32)

#include <WiFi.h>
#include <InternalStorageESP.h>

#elif defined(ARDUINO_SAMD_NANO_33_IOT)

#include <WiFiNINA.h>
#include <InternalStorage.h>

#endif

#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <SPI.h>

class aa_lib {
private:

    /**
     * The current wifi connectivity status
     */
    int status = WL_IDLE_STATUS;

    /**
     * The JSON variable that contains the last state of the arduino.
     */
    StaticJsonDocument<512> doc;

    /**
     * The current version of the software (used for OTA checking)
     */
    short version;

    /**
     * The type of device this is (ie, fishtank, rockgarden)
     */
    char* device_type;

    /**
     * The sever to check for updates at.
     */
    char* update_server;

    /**
     * The API key for the arduino automation service.
     */
    char* api_key;

    /**
     * The API User ID for the arduino automation service.
     */
    char* api_id;

    /**
     * The port used to connect to the update server.
     */
    unsigned short server_port;

    /**
     * The SSID to connect to.
     */
    char *wifi_ssid;

    /**
     * The WiFi password.
     */
    char *wifi_password;

public:

    /**
     * Creates a new arduino automation connection for getting the last state/updating the arduino.
     * @param ssid - The WiFi SSID to connect to.
     * @param pass - The WiFi password.
     * @param type - What the type of this device is
     * @param server - The server to check for updates.
     * @param port - The port for the server.
     * @param ver - The version of software to report to the server.
     * @param _api_key - The API key used to communicate with an arduino automation service.
     * @param _api_id - The user ID used to communicate with an arduino automation service.
     */
    aa_lib(char ssid[],char pass[], char* type, char* server, unsigned short port, short ver, char* _api_key, char* _api_id);

     /**
     * Gets the last state of the given GUID.
     * @param - The GUID to check.
     */
    bool getLastState(String guid);

    /**
     * Checks for an update to the arduino
     */
    void handleSketchDownload();

    /**
     * Checks if WiFi is connected, and if it is not, connect the arduino to the network.
     */
    void checkAndConnectWifi();

    /**
     * Prints the current IP address of the arduino.
     */
    void printIP();

    /**
     * Sets up aa_lib for use.
     */
    void setup();

    /**
     * Gets the current IP address of the arduino.
     */
    String getIP();

    /**
     * Gets whether or not the WiFiServer used for receiving post arguments is available.
     */
    bool wifiServerAvailable();

    /**
     * Updates the device in the database.
     * @param guid - the GUID we are updating.
     * @param state - the new state of the arduino.
     */
    void sendUpdate(String guid, bool state);
};
#endif
