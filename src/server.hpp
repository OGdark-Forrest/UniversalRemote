#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebServer.h>
#include "secrets.hpp"
#include "general.hpp"
#include "irHandler.hpp"

class wifiHandler{
    public:
        wifiHandler();
        bool checkConnection(){
            return WiFi.isConnected();
        }
        status retryConnection(){
            while(checkConnection() == false){
                status connectionStatus = tryConnection();
                if(connectionStatus.Code != 200){return connectionStatus;}
                delay(500);
            }
            return {
                "Reconnected to the WiFi",
                200,
                "SSID: " + std::string(this->SSID),
                ""
            };
        }

    private:
        const char* SSID = networkSSID.c_str();
        const char* PASSWORD = networkPassword.c_str();

        status tryConnection(){
            return tryFunction("WifiConnection", [this]{
                WiFi.begin(this->SSID, this->PASSWORD);
            });
        }
};

class serverHandler{
    public:
        serverHandler(int portNumber, irHandler* irManagerPointer, serverState status);
    
    private:
        AsyncWebServer asyncServerObj;
        WebServer syncServerObj;
        irHandler& irManager;
        serverState status;

        void addRoutes();

        void addCommand(AsyncWebServerRequest* request);
        void addSignal(AsyncWebServerRequest* request);
        void toggleOffCloning(AsyncWebServerRequest* request);
};