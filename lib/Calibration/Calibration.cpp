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

void WebServer::init_webserver(Data *data){
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    this->server = new AsyncWebServer(80);

    this->server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "API Not Found");
    });

    // API REQUESTS
    this->server->on("/api/refresh", HTTP_GET, [data](AsyncWebServerRequest *request) {
        auto response = new AsyncJsonResponse();
        auto json = response->getRoot();

        json["temperature"] = data->temperature;
        json["humidity"] = data->humidity;
        json["pressure"] = data->pressure;
        json["wind_speed"] = data->wind_speed;
        json["wind_direction"] = data->wind_direction;

        response->setLength();
        response->setCode(200);

        request->send(response);

        delete response;
    });

    this->server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    this->server->begin();
}