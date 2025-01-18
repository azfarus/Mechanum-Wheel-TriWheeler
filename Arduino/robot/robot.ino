#pragma once
#include "headers/servo_utils.h"
#include "headers/controller_utils.h"
#include "headers/motor_utils.h"
#include "headers/led.h"
#include <WiFi.h>
#include<FastLED.h>



BLA::Matrix<1, 3, float> coefficients;



void setup(){
    Serial.begin(115200);

    pinMode(LED_BUILTIN,OUTPUT);
    servo_init();
    startup_prints();
    controller_startup();

    coefficients.Fill(0.0f);

    ledMatrixSetup();

}

void loop(){

    bool dataUpdated = BP32.update();
    if (dataUpdated){
        coefficients = getCoefficents();
    }

   
    set_motor_speed(0+13 , coefficients(0,0));
    set_motor_speed(1+13 , coefficients(0,1));
    set_motor_speed(2+13 , coefficients(0,2));

    vTaskDelay(10/portTICK_PERIOD_MS);

}


