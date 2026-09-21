#pragma once

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

#include "general.hpp"

class irHandler{
    public:
        irHandler(const uint16_t rPin, const uint16_t sPin);
        status listen();
        status send();

        void toggleCloningOff(){
            this->isCloning = false;
        }
        void toggleCloningOn(){
            this->isCloning = true;
        }

        void signalQueueOperation(int operation, jobInfo signal = {}){
            if(operation == 1){
                (this->signalQueue).push_back(signal);
            }
            else if(operation == -1){
                (this->signalQueue).erase((this->signalQueue).begin());
            }
        }

        void commandQueueOperation(int operation, jobInfo signal = {}){
            if(operation == 1){
                (this->commandQueue).push_back(signal);
            }
            else if(operation == -1){
                (this->commandQueue).erase((this->commandQueue).begin());
            }
        }

    private:
        const uint16_t receivePin;
        const uint16_t sendPin;

        IRsend irsend;
        IRrecv irrecv;
        
        decode_results results;
        bool isCloning;

        std::vector<jobInfo> commandQueue;
        std::vector<jobInfo> signalQueue;

        status sendSignal(std::string deviceName, std::string commandName);
        status cloneSignal(std::string deviceName, std::string commandName);
};