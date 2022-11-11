AsyncWebServer server(80);
const char* ssid = "안녕ESP";
const char* password = "";

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";

String SSIDstr = "";
String PSWDstr = "";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    SSID: <input type="text" name="input1">
    PSWD: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void serverInit() {
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      SSIDstr = request->getParam(PARAM_INPUT_1)->value();
      PSWDstr = request->getParam(PARAM_INPUT_2)->value();
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    Serial.println(SSIDstr);
    Serial.println(PSWDstr);
    //http://192.168.4.1/get?input1=${SSID}&input2=${PSWD}
    //be registed like this.
    /*
      request->send(200, "text/html", "SSID: "
                                     + SSIDstr + "<br>PSWD: "+"" + PSWDstr +
                                     "<br><a href=\"/\">Return to Home Page</a>");
    */
    request->redirect("/");
  });
}

void confirmEEPROM() {
  EEPROM.begin(100);
  SSIDstr = EEPROM.readString(0);
  PSWDstr = EEPROM.readString(20);
  if (SSIDstr == "")
  {
    WiFi.softAP(ssid, password);
    Serial.println();
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    serverInit();
    server.onNotFound(notFound);
    server.begin();
  }
}

void wifiCon() {
  const char* clientSsid = SSIDstr.c_str();
  const char* clientPswd = PSWDstr.c_str();
  if (SSIDstr != "" && EEPROM.readString(0) == "") {

    //WiFi.disconnect(true);
    //WiFi.mode(WIFI_OFF);
    //WiFi.mode(WIFI_STA);

    WiFi.begin(clientSsid, clientPswd);
    for (int i = 0; i < 10; i++) {
      if (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("CONNECTED.");
      Serial.println(WiFi.localIP());
      EEPROM.writeString(0, clientSsid);
      EEPROM.writeString(20, clientPswd);
      EEPROM.commit();
      ESP.restart();
    }
    else {
      Serial.println("Fail.");
      WiFi.disconnect();
      WiFi.softAP(ssid, password);
      SSIDstr = "";
      PSWDstr = "";
    }
  }
  if (EEPROM.readString(0) != "" && WiFi.status() != WL_CONNECTED) {
    WiFi.begin(clientSsid, clientPswd);
    if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("CONNECTED.");
      Serial.println(WiFi.localIP());
    }
  }
}
