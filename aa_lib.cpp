#include <aa_lib.h>

aa_lib::aa_lib(char ssid[], char pass[], char* type, char* server, unsigned short port, short ver, char* _api_key, char* _api_id){
    version = ver;
    device_type = type;
    update_server = server;
    api_key = _api_key;
    api_id = _api_id;
    server_port = port;
    wifi_ssid = ssid;
    wifi_password = pass;
}

void aa_lib::setup() {
#if defined(ESP32)
    WiFi.mode(WIFI_STA);
#endif
    checkAndConnectWifi();
}

void aa_lib::printIP() {
    Serial.print("IP Address: ");
    Serial.println(getIP());
}

String aa_lib::getIP()
{
  const IPAddress& ipAddress = WiFi.localIP();
    return String(ipAddress[0]) + String(".") +\
        String(ipAddress[1]) + String(".") +\
        String(ipAddress[2]) + String(".") +\
        String(ipAddress[3])  ;
}

void aa_lib::checkAndConnectWifi() {
    status = WiFi.status();
    if(status != WL_CONNECTED){
        #if !defined(ESP32)
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        #endif
        Serial.print("WiFi disconnected, connecting to wifi.");
        while(status != WL_CONNECTED){
            status = WiFi.begin(wifi_ssid,wifi_password);
            Serial.print(".");
            delay(3000);
        }
        Serial.println("");
        Serial.println("Connected");
        #if !defined(ESP32)
        digitalWrite(LED_BUILTIN, LOW);
        #endif
        printIP();
    }
}

void aa_lib::sendUpdate(String guid, bool state){
    WiFiClient wifi;
    HttpClient httpClient = HttpClient(wifi, update_server, server_port);
    String contentType = "application/x-www-form-urlencoded";
    String data = "guid=" + guid + "&ip=" + getIP() + "&state=" + (state ? "true" : "false") + "&sw_version=" + String(version);
    httpClient.sendHeader("x-api-key", api_key);
    httpClient.sendHeader("x-auth-id", api_id);
    httpClient.sendHeader(HTTP_HEADER_CONTENT_LENGTH, data.length());
    Serial.println(data);
    httpClient.put("/smarthome/update",contentType,data);
    int statusCode = httpClient.responseStatusCode(); 
    httpClient.stop();
}

bool aa_lib::getLastState(String guid){
    WiFiClient wifi;
    Serial.println("Getting last state for " + guid);
    Serial.println(update_server);
    Serial.println(server_port);
    HttpClient httpClient = HttpClient(wifi, update_server, server_port);
    httpClient.beginRequest();
    httpClient.get("/smarthome/device/"+guid);
    Serial.println(api_key);
    Serial.println(api_id);
    httpClient.sendHeader("x-api-key", api_key);
    httpClient.sendHeader("x-auth-id", api_id);
    httpClient.endRequest();
    String responeBody = httpClient.responseBody();
    DeserializationError error = deserializeJson(doc, responeBody.c_str(), 512);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    }
    Serial.println(responeBody);
    return doc["last_state"];
}

void aa_lib::handleSketchDownload(){
    const char* PATH = "/ota/arduino/update-%s-%d.bin";
    const unsigned long CHECK_INTERVAL = 60000;

    static unsigned long previousMillis;

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis < CHECK_INTERVAL)
        return;
    previousMillis = currentMillis;

    WiFiClient transport;
    HttpClient client(transport, update_server, server_port);
    Serial.println(String(update_server) + String(PATH));
    char buff[50];
    snprintf(buff, sizeof(buff), PATH, device_type, version + 1);

    Serial.print("Check for update file ");
    Serial.println(buff);

    client.get(buff);

    int statusCode = client.responseStatusCode();
    Serial.print("Update status code: ");
    Serial.println(statusCode);
    if (statusCode != 200) {
        client.stop();
        return;
    }

    long length = client.contentLength();
    if (length == HttpClient::kNoContentLengthHeader) {
        client.stop();
        Serial.println("Server didn't provide Content-length header. Can't continue with update.");
        return;
    }
    Serial.print("Server returned update file of size ");
    Serial.print(length);
    Serial.println(" bytes");

    if (!InternalStorage.open(length)) {
        client.stop();
        Serial.println("There is not enough space to store the update. Can't continue with update.");
        return;
    }
    byte b;
    while (length > 0) {
        if (!client.readBytes(&b, 1)) // reading a byte with timeout
            break;
        InternalStorage.write(b);
        length--;
    }
    InternalStorage.close();
    client.stop();
    if (length > 0) {
        Serial.print("Timeout downloading update file at ");
        Serial.print(length);
        Serial.println(" bytes. Can't continue with update.");
        return;
    }

    Serial.println("Sketch update apply and reset.");
    Serial.flush();
    InternalStorage.apply(); // this doesn't return
}

