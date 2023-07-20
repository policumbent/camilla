# pragma once

#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FeedbackStepper.h>
#include <Potentiometer.h>
#include <Memory.h>

#define AP_SSID "***"
#define AP_PASSWORD "***"

class WebServer {

    AsyncWebServer *server;
    FeedbackStepper *stepper_motor;
    Potentiometer *linear_potentiometer;
    Memory *mem;

public:
    WebServer(FeedbackStepper *stepper_motor, Potentiometer *linear_potentiometer, Memory *mem);
    ~ WebServer();

private:
    void init_webserver();
    void save_gear(int gear);
    String get_gear_position();

};