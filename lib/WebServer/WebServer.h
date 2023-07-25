# pragma once

#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <WiFiAP.h>

#include "FeedbackStepper.h"
#include "Potentiometer.h"
#include "Memory.h"


#define WEBSERVER_AP_SSID "ESP_CAMILLA"
#define WEBSERVER_AP_PASSWORD "camilla123"

class WebServer {

    private:
        AsyncWebServer *server;
        FeedbackStepper *stepper_motor;
        Potentiometer *linear_potentiometer;
        int *gears;
        int *gears_lin;
        int num_gears;
        SemaphoreHandle_t *semaphore;

    public:
        WebServer(FeedbackStepper *stepper_motor, Potentiometer *linear_potentiometer, int *gears, int *gears_lin, int num_gears, SemaphoreHandle_t *semaphore);
        ~ WebServer();

    private:
        void init_webserver();
        void save_gear(int gear);
        String get_gear_position(int gear);
        String get_current_position();

};