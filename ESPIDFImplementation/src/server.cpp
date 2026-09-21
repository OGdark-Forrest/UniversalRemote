#include "server.hpp"

serverHandler::serverHandler(int portNumber, irHandler* irManagerPointer, serverState state):
asyncServerObj(portNumber), syncServerObj(portNumber),irManager(*irManagerPointer), status(state)
{
    this->addRoutes();
    if(state == serverState::ASYNC){
        (this->asyncServerObj).begin();
    }
    else{
        (this->syncServerObj).begin();
    }
    (this->irManager.toggleCloningOn());
}

// =================================
// Asynchronous Overloaded Functions
// =================================

void serverHandler::initiateConnection(AsyncWebServerRequest* request){
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

// ================================
// Synchronous Overloaded Functions
// ================================

void serverHandler::initiateConnection(){
    (this->syncServerObj).send(200, "text/plain", "Connected successfully");
}

void serverHandler::addSignal(){
    if((this->syncServerObj).hasArg("deviceName") == false){
        (this->syncServerObj).send(400, "text/plain", "Device Name is missing");
        return;
    }
    if((this->syncServerObj).hasArg("commandName") == false){
        (this->syncServerObj).send(400, "text/plain", "Command Name is missing");
        return;
    }

    this->irManager.signalQueueOperation(
        1, 
        {
            (this->syncServerObj).arg("deviceName").c_str(),
            (this->syncServerObj).arg("commandName").c_str()
        }
    );
    (this->syncServerObj).send(200, "text/plain", "Successfully added signal to send");
}

void serverHandler::addCommand(){
    if((this->syncServerObj).hasArg("deviceName") == false){
        (this->syncServerObj).send(400, "text/plain", "Device Name is missing");
        return;
    }
    if((this->syncServerObj).hasArg("commandName") == false){
        (this->syncServerObj).send(400, "text/plain", "Command Name is missing");
        return;
    }

    this->irManager.toggleCloningOn();

    this->irManager.commandQueueOperation(
        1,
        {
            (this->syncServerObj).arg("deviceName").c_str(),
            (this->syncServerObj).arg("commandName").c_str()
        }
    );

    (this->syncServerObj).send(200, "text/plain", "Successfully added command to register");
}

void serverHandler::toggleOffCloning(){
    this->irManager.toggleCloningOff();
    (this->syncServerObj).send(200, "text/plain", "Toggled cloning mode off");
}

void serverHandler::addRoutes(){
    if(this->status == serverState::ASYNC){
        asyncServerObj.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
            this->initiateConnection(request);
        });
        asyncServerObj.on("/addSignal", HTTP_PUT, [this](AsyncWebServerRequest *request) {
            this->addSignal(request);
        });
        asyncServerObj.on("/addCommand", HTTP_POST, [this](AsyncWebServerRequest *request) {
            this->addCommand(request);
        });
        asyncServerObj.on("/cloningOff", HTTP_PATCH, [this](AsyncWebServerRequest *request) {
            this->toggleOffCloning(request);
        });

        return;
    }

    syncServerObj.on("/", HTTP_GET, [this]() {
        this->initiateConnection();
    });
    syncServerObj.on("/addSignal", HTTP_PUT, [this]() {
        this->addSignal();
    });
    syncServerObj.on("/addCommand", HTTP_POST, [this]() {
        this->addCommand();
    });
    syncServerObj.on("/cloningOff", HTTP_PATCH, [this]() {
        this->toggleOffCloning();
    });
    
}

wifiHandler::wifiHandler(){
    this->tryConnection();
}

