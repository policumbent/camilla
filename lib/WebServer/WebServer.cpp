#include "WebServer.h"
#include <WiFi.h>
#include <WiFiAP.h>
#include <Arduino.h>

WebServer::WebServer(FeedbackStepper *stepper_motor, Potentiometer *linear_potentiometer, Memory *mem) {
    this->stepper_motor = stepper_motor;
    this->linear_potentiometer = linear_potentiometer;
    this->mem = mem;
    
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
    this->server->on("/api/get_gear", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", get_gear_position());
    });

    this->server->on("/api/position", HTTP_GET, [this](AsyncWebServerRequest *request){
        char response[20]; 
        sprintf(response, "%d;%d", linear_potentiometer->read_position(), stepper_motor->get_position());
        request->send(200, "text/plain", response);
    });

    this->server->on("/api/calibrate", HTTP_GET, [this](AsyncWebServerRequest *request){
        if(request->hasArg("gear")){
            String arg = request->arg("gear");
            save_gear(arg.toInt());
        }

    });

    this->server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    this->server->begin();
}

void WebServer::save_gear(int gear){

}

String WebServer::get_gear_position(){

}