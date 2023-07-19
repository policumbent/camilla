# pragma once

#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WebServer {

    AsyncWebServer *server;

public:
    WebServer();
    ~ WebServer();

private:
    void init_webserver();

};