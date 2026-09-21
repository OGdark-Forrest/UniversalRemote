#pragma once

#include <ArduinoJson.h>
#include <any>

struct status {
    std::string StatusMessage;
    uint16_t Code;
    std::string Detail;
    std::any Payload;
};

struct jobInfo {
    std::string deviceName;
    std::string commandName;
};

template <typename F, typename... Args>
status tryFunction(std::string funcName, F func, Args&&... args) {
    try{
        using ReturnType = decltype(func(std::forward<Args>(args)...));
        if constexpr(std::is_same<ReturnType, status>::value){
            return func(std::forward<Args>(args)...);
        }
        else{
            func(std::forward<Args>(args)...);
            return {
                "Successfully Called Function " + funcName,
                200,
                "",
                ""
            };
        }
    }
    catch (const std::exception& e){
        return {
            "Unexpected Error Occurred during calling of " + funcName,
            500,
            std::string(e.what()),
            ""
        };
    }
}

inline status textToJSON(std::string content){
    JsonDocument jsonObj;
    DeserializationError error = deserializeJson(jsonObj, content);

    if(error){
        return {
            "Deserialization failure",
            500,
            error.c_str(),
            ""
        };
    }

    return {
        "Deserialized file to JSON object",
        200,
        "",
        jsonObj
    };
}

inline status jsonToText(JsonDocument jsonObj){
    std::string content;
    serializeJson(jsonObj, content);

    return {
        "Serialized JSON object to text",
        200,
        "",
        content
    };
}
