#pragma once

#include <LittleFS.h>
#include "general.hpp"

inline status writeFile(const std::string fileName, std::string content){
    File wFile = LittleFS.open(fileName.c_str(), "w");

    wFile.print(content.c_str());

    wFile.close();
    return {
        "File succesfully written to",
        200,
        "Filename " + fileName + " has been written to",
        content
    };
}

inline status readFile(std::string fileName){
    std::string content = "";
    File rFile = LittleFS.open(fileName.c_str(), "r");

    if(!rFile){
        rFile.close();
        return {
            "File doesn't exist",
            404,
            "Filename " + fileName + " has not been found",
            ""
        };
    }
    if(rFile.isDirectory()){
        rFile.close();
        return {
            "File doesn't exist",
            406,
            "Filename " + fileName + " is a directory",
            ""
        };
    }

    while(rFile and rFile.available()){
        char byte = (char)rFile.read();
        content += byte;
    }

    rFile.close();

    return {
        "File successfully read",
        200,
        "Filename " + fileName + " successfully written to",
        content
    };
}