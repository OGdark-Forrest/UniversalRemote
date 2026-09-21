#include "irHandler.hpp"
#include "fileHandler.hpp"

irHandler::irHandler(const uint16_t rPin, const uint16_t sPin):
receivePin(rPin), sendPin(sPin), irsend(sPin), irrecv(rPin)
{
    (this->irsend).begin();
    (this->irrecv).enableIRIn();
}

status checkValidDetails(JsonDocument* signalInfo, std::string deviceName, std::string commandName){
    if((*signalInfo)[deviceName].is<JsonObject>() == false){
        return {
            "Signal send failed",
            400,
            "Device " + deviceName + " not found in record",
            ""
        };
    }
    if((*signalInfo)[deviceName][commandName].is<JsonObject>() == false){
        return {
            "Signal send failed",
            400,
            "Command " + commandName + " not found under device " + deviceName,
            ""
        };
    }

    return {
        "Valid Fields",
        200,
        "",
        ""
    };
}

status getSignalInfo(){
    status readStatus = readFile("signals.json");
    if(readStatus.Code != 200){return readStatus;}

    status deserializeStatus = textToJSON(std::any_cast<std::string>(readStatus.Payload));
    return deserializeStatus;
}

status writeSignalInfo(JsonDocument signalInfo){
    status serializeStatus = jsonToText(signalInfo);
    if(serializeStatus.Code != 200){return serializeStatus;}

    status writeStatus = writeFile("signals.json", std::any_cast<std::string>(serializeStatus.Payload));
    return writeStatus;
}

status irHandler::cloneSignal(std::string deviceName, std::string commandName){
    status deserializationStatus = getSignalInfo();
    if(deserializationStatus.Code != 200){return deserializationStatus;}

    JsonDocument signalInfo = std::any_cast<JsonDocument>(deserializationStatus.Payload);
    status validityStatus = checkValidDetails(&signalInfo, deviceName, commandName);

    if(validityStatus.Code == 200){
        return {
            "Fields already exist in the register",
            400,
            "Device Name: "+deviceName+", Command Name: "+commandName,
            ""
        };
    }

    signalInfo[deviceName][commandName]["protocol"] = (int)this->results.decode_type;
    signalInfo[deviceName][commandName]["value"] = this->results.value;
    signalInfo[deviceName][commandName]["bits"] = this->results.bits;

    status writeStatus = writeSignalInfo(signalInfo);
    if(writeStatus.Code != 200){return writeStatus;}

    return {
        "Succesfully added signal",
        200,
        "Added signal for command " + commandName + " for device " + deviceName,
        ""
    };
}

status irHandler::sendSignal(std::string deviceName, std::string commandName){
    status deserializationStatus = getSignalInfo();
    if(deserializationStatus.Code != 200){return deserializationStatus;}
    
    JsonDocument signalInfo = std::any_cast<JsonDocument>(deserializationStatus.Payload);
    status validityStatus = checkValidDetails(&signalInfo, deviceName, commandName);

    if(validityStatus.Code != 200){
        return validityStatus;
    }

    JsonObject signalData = signalInfo[deviceName][commandName].as<JsonObject>();

    decode_type_t protocol = (decode_type_t)signalData["protocol"].as<int>();
    uint64_t value = signalData["value"].as<uint64_t>();
    uint16_t bits = signalData["bits"].as<uint16_t>();

    status sendStatus = tryFunction("IR transmission", [this, protocol, value, bits](){
        this->irsend.send(protocol, value, bits);
    });
    if(sendStatus.Code != 200){return sendStatus;}

    this->signalQueueOperation(-1);

    return {
        "Signal sent successfully",
        200,
        "Device Name: "+deviceName+", Command Name: "+commandName,
        ""
    };
}

status irHandler::send(){
    if((this->signalQueue).empty()){
        return {
            "Nothing to send",
            200,
            "",
            ""
        };
    }

    jobInfo currSignal = (this->signalQueue).at(0);
    return sendSignal(currSignal.deviceName, currSignal.commandName);
}

status irHandler::listen(){
    if(this->isCloning == false or (this->commandQueue).empty()){
        return {
            "Not in cloning mode/Empty queue",
            200,
            "",
            ""
        };
    }

    status listenStatus = tryFunction("IR reception", [this](){
        this->irrecv.decode(&(this->results));
    });
    if(listenStatus.Code != 200){return listenStatus;}
    jobInfo currCommand = this->commandQueue.at(0);

    status cloneStatus = cloneSignal(currCommand.deviceName, currCommand.commandName);
    if(cloneStatus.Code != 200){return cloneStatus;}

    (this->commandQueue).erase((this->commandQueue).begin());
    return {
        "Command added and removed from the queue",
        200,
        "Device Name: "+currCommand.deviceName+", Command Name: "+currCommand.commandName,
        ""
    };
}