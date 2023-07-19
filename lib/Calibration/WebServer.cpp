#include "WebServer.h"
#include <WiFi.h>
#include <WiFiAP.h>

WebServer::WebServer() {
    init_webserver();
}

WebServer::~WebServer() {
    server->end();
    server->reset();
    SPIFFS.end(); // this may cause problems TODO: test
    WiFi.softAPdisconnect(true);
    delete server;
}

void WebServer::init_webserver(){
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    this->server = new AsyncWebServer(80);

    this->server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "API Not Found");
    });

    // API REQUESTS
    this->server->on("/api/get_gear", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", get_gear_position());
    });

    this->server->on("/api/position", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", get_linear() + ";" + get_rotative());
    });

    this->server->on("/api/calibrate", HTTP_GET, [](AsyncWebServerRequest *request){
        calibrate(request->getParam("gear"), get_linear(), get_rotative());
    });

    this->server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    this->server->begin();
}