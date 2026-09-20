#include "server.hpp"

serverHandler::serverHandler(int portNumber, irHandler* irManagerPointer):
serverObj(portNumber), irManager(*irManagerPointer)
{
    (this->serverObj).begin();
    (this->irManager.toggleCloningOn());
    this->addRoutes();
}

void initiateConnection(AsyncWebServerRequest* request){
    request->send(200, "text/plain", "Connected successfully");
}

void serverHandler::addSignal(AsyncWebServerRequest* request){
    if(request->hasParam("deviceName") == false){
        request->send(400, "text/plain", "Device Name is missing");
        return;
    }
    if(request->hasParam("commandName") == false){
        request->send(400, "text/plain", "Command Name is missing");
        return;
    }

    this->irManager.signalQueueOperation(
        1, 
        {
            request->getParam("deviceName")->value().c_str(),
            request->getParam("commandName")->value().c_str()
        }
    );
    request->send(200, "text/plain", "Successfully added signal to send");
}

void serverHandler::addCommand(AsyncWebServerRequest* request){
    if(request->hasParam("deviceName") == false){
        request->send(400, "text/plain", "Device Name is missing");
        return;
    }
    if(request->hasParam("commandName") == false){
        request->send(400, "text/plain", "Command Name is missing");
        return;
    }

    this->irManager.toggleCloningOn();

    this->irManager.commandQueueOperation(
        1,
        {
            request->getParam("deviceName")->value().c_str(),
            request->getParam("commandName")->value().c_str()
        }
    );

    request->send(200, "text/plain", "Successfully added command to register");
}

void serverHandler::toggleOffCloning(AsyncWebServerRequest* request){
    this->irManager.toggleCloningOff();
    request->send(200, "text/plain", "Toggled cloning mode off");
}

void serverHandler::addRoutes(){
    (this->serverObj).on(
        "/",
        HTTP_GET,
        initiateConnection
    );

    serverObj.on("/addSignal", HTTP_PUT, [this](AsyncWebServerRequest *request) {
        this->addSignal(request);
    });

    serverObj.on("/addCommand", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->addCommand(request);
    });

    serverObj.on("/cloningOff", HTTP_PATCH, [this](AsyncWebServerRequest *request) {
        this->toggleOffCloning(request);
    });
}

wifiHandler::wifiHandler(){
    this->tryConnection();
}

