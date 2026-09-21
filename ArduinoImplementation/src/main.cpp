#include <Arduino.h>
#include "irHandler.hpp"
#include "fileHandler.hpp"
#include "server.hpp"
#include "general.hpp"

irHandler* irManager;
serverHandler* serverManager;
wifiHandler* wifiManager;

void setup(){
  Serial.begin(115200);
  Serial.println("Boot: starting");

  LittleFS.begin();
  Serial.println("Started LittleFS");

  irManager = new irHandler(12, 2);
  Serial.println("irHandler ready");

  wifiManager = new wifiHandler();
  Serial.println("WiFi handler ready");

  serverManager = new serverHandler(80, irManager, serverHandler::serverState::SYNC);
  Serial.println("Server ready");
}

void loop(){
  if(wifiManager->checkConnection() == false){
    Serial.println("Connection failed, retrying");
    wifiManager->retryConnection();
  }
  irManager->listen();
  irManager->send();

  delay(500);
}
