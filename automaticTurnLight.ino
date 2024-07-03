#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>

const char* ssid = "controleLuz";
const char* passWord = "123456789";

int sensorPin = 0;

int presenceState = 0;
int lightState = 1;
int useSwitch = 1;

AsyncWebServer server(80);

void setup() {
  Serial.begin(9600);

  /*Inicia o WiFi no modo de soft acess point que permite ao usuario acessar a ESP8266 sabendo a senha e o SSID
  */
  WiFi.softAP(ssid, passWord);
  IPAddress IP = WiFi.softAPIP();

  Serial.println(IP);
  Serial.println(WiFi.localIP()); 

  pinMode(sensorPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //Inicia o server
  server.begin();

  //inicia o sistema de file do ESP
  if(!LittleFS.begin()){
    Serial.println("Erro ao abrir littleFS"); return;
  }

  //Trata todos os pedidos de arquivo ou de estado
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html");
    Serial.println("Sucess html");
  });

  server.on("/look.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/look.css");
    Serial.println("Sucess css");
  });

  server.on("/control.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/control.js");
    Serial.println("Sucess js");
  });

  server.on("/lightState", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("lighstate asked");
    if(lightState == 0) request->send_P(200, "text/plain", "0");
    if(lightState == 1) request->send_P(200, "text/plain", "1");
  });

  server.on("/lightON", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "ok");
    Serial.println("light on asked");
    useSwitch = 0;
    lightState = 0;
  });

  server.on("/lightOFF", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("light off asked");
    request->send_P(200, "text/plain", "ok");
    useSwitch = 1;
    lightState = 1;
  });
}

void loop() {
  presenceState = digitalRead(sensorPin);
  if(useSwitch == 1) lightState = presenceState;
  if(lightState == 0){
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
