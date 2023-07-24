#include "WebServer.h"


WebServer :: WebServer(FeedbackStepper *stepper_motor, Potentiometer *linear_potentiometer, int *gears, int *gears_lin, int num_gears, SemaphoreHandle_t *semaphore) {
    this->stepper_motor = stepper_motor;
    this->linear_potentiometer = linear_potentiometer;
    this->gears = gears;
    this->gears_lin;
    this->num_gears = num_gears;
    this->semaphore = semaphore;
    
    init_webserver();
}

WebServer :: ~ WebServer() {
    server->end();
    server->reset();
    SPIFFS.end(); // this may cause problems TODO: test
    WiFi.softAPdisconnect(true);
    delete server;
}

void WebServer :: init_webserver() {
    SPIFFS.begin();

    WiFi.softAP(AP_SSID, AP_PASSWORD);

    this->server = new AsyncWebServer(80);

    this->server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "API Not Found");
    });

    // API REQUESTS
    this->server->on("/api/get_gear", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if(request->hasArg("gear"))
            request->send(200, "text/plain", get_gear_position(request->arg("gear").toInt()));
    });

    this->server->on("/api/position", HTTP_GET, [this](AsyncWebServerRequest *request){
        char response[20]; 
        sprintf(response, "%d,%d", linear_potentiometer->read_position(), stepper_motor->get_position());
        request->send(200, "text/plain", response);
    });

    this->server->on("/api/calibrate", HTTP_GET, [this](AsyncWebServerRequest *request){
        if(request->hasArg("gear")){
            String arg = request->arg("gear");
            save_gear(arg.toInt());
            request->send(200, "text/plain", get_gear_position(arg.toInt()));
        }
        
    });

    this->server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    this->server->begin();
}

void WebServer :: save_gear(int gear) {
    xSemaphoreTake(*semaphore, portMAX_DELAY);

    gears[gear-1] = stepper_motor->get_position();
    gears_lin[gear-1] = linear_potentiometer->get_position();

    xSemaphoreGive(*semaphore);
}

String WebServer :: get_gear_position(int gear) {
    String tmp;

    xSemaphoreTake(*semaphore, portMAX_DELAY);

    tmp = String(gears_lin[gear-1]) + String(",") + String(gears[gear-1]);

    xSemaphoreGive(*semaphore);

    return tmp;
}