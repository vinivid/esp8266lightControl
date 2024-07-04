#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

//Nome e senha da rede para conectar na ESP
const char* ssid = "controleLuz";
const char* passWord = "123456789";

//Pino que controla a luz através do transitor
int controlLight = 13;

int lightState = 1;

int useSwitch = 1;
int switchPress = 0;

AsyncWebServer server(80);

//Variaveis relacionadas ao pino sensor e quantidade de vezes que foi utilizado
int sensorPin = 0;
int presenceState = 0;
int countSensorQt = 0;
int sensorCountState = 0;

//variaveis que contão o tempo que uma lampada ficou ligada
long int timeLightOn = 0;
int timeCountStart = 0;
long int miliP1 = 0;
long int miliP2 = 0;

/*A função readJSON() é executada uma vez no programa e sua função é apenas ler os dados armazenados
no JSON e coloca-los em suas devidas variaveis de contagem
*/
void readJSON(){
  File storeFile = LittleFS.open("/store.json", "r");
  if(!(storeFile)){
    Serial.println("Falha ao abrir storage"); return;
  }

  DynamicJsonDocument storage(1024);
  deserializeJson(storage, storeFile);
  storeFile.close();

  timeLightOn = storage["time"];
  countSensorQt = storage["sensor"];
  switchPress = storage["switches"];
}

/*A função saveJSON() é usada 
*/
void saveJSON(){
  File storeFile = LittleFS.open("/store.json", "w");
  if(!(storeFile)){
    Serial.println("Falha ao abrir storage"); return;
  }

  DynamicJsonDocument storage(1024);

  storage["time"] = timeLightOn;
  storage["sensor"] = countSensorQt;
  storage["switches"] = switchPress;

  serializeJson(storage, storeFile);
  storeFile.close();
}

void setup() {
  Serial.begin(9600);

  /*Inicia o WiFi no modo de soft acess point que permite ao usuario acessar a ESP8266 sabendo a senha e o SSID
  */
  WiFi.softAP(ssid, passWord);
  IPAddress IP = WiFi.softAPIP();

  Serial.println(IP);

  pinMode(sensorPin, INPUT);
  pinMode(controlLight, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //Inicia o server
  server.begin();

  //inicia o sistema de file do ESP
  if(!LittleFS.begin()){
    Serial.println("Erro ao abrir littleFS"); return;
  }

  //lê o que havia sido salvo previamente
  readJSON();

  //Envia o html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html");
    Serial.println("Sucess html");
  });

  //Envia o css
  server.on("/look.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/look.css");
    Serial.println("Sucess css");
  });

  //Envia o javascript
  server.on("/control.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/control.js");
    Serial.println("Sucess js");
  });

  //Antes de enviar para a pagina atualiza o que estava no JSON
  server.on("/store", HTTP_GET, [](AsyncWebServerRequest *request){
    saveJSON();
    request->send(LittleFS, "/store.json");
    Serial.println("Sucess json");
  });

  //Envia o texto para a pagina com base no estado da lâmpada
  server.on("/lightState", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("lighstate asked");
    if(lightState == 0) request->send_P(200, "text/plain", "0");
    if(lightState == 1) request->send_P(200, "text/plain", "1");
  });

  //Quando a luz é ligada pelo interruptor remoto
  server.on("/lightON", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "ok");
    Serial.println("light on asked");
    useSwitch = 0;
    lightState = 0;
    switchPress += 1;
  });

  //Quando a luz é desligada pelo interruptor remoto
  server.on("/lightOFF", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("light off asked");
    request->send_P(200, "text/plain", "ok");
    useSwitch = 1;
    lightState = 1;
    switchPress += 1;
  });
}

void loop() {
  //Vê se o sensor esta detectando algo
  presenceState = digitalRead(sensorPin);

  //Lógica para testar quantas vezes o sensor foi usado
  if(useSwitch == 1){
    lightState = presenceState;
    if(sensorCountState == 0 && lightState == 0){
      countSensorQt += 1;
      sensorCountState = 1;
    }else if(sensorCountState == 1 && lightState == 1){
      sensorCountState = 0;
    }
  }else{
    sensorCountState = 0;
  }

  //Contador de tempo em que a luz ficou acessa em mili segundos, também liga ou desliga a luz
  if(lightState == 0){
    if(timeCountStart == 0){
      miliP1 = millis();
      timeCountStart = 1;
    }
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(controlLight, HIGH);
  }else{
    if(timeCountStart == 1){
      miliP2 = millis();
      timeLightOn += miliP2-miliP1;
      Serial.println(timeLightOn);
      timeCountStart = 0;
    }
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(controlLight, LOW);
  }
}
