#include <Arduino.h>
#line 1 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\robot.ino"
#pragma once
#include "headers/servo_utils.h"
#include "headers/controller_utils.h"
#include "headers/motor_utils.h"
#include "headers/led.h"
#include <WiFi.h>
#include<FastLED.h>



BLA::Matrix<1, 3, float> coefficients;



#line 15 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\robot.ino"
void setup();
#line 29 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\robot.ino"
void loop();
#line 7 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\controller_utils.ino"
void onConnectedController(ControllerPtr ctl);
#line 27 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\controller_utils.ino"
void onDisconnectedController(ControllerPtr ctl);
#line 44 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\controller_utils.ino"
void dumpGamepad(ControllerPtr ctl);
#line 61 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\controller_utils.ino"
void processGamepad(ControllerPtr ctl);
#line 66 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\controller_utils.ino"
void processControllers();
#line 78 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\controller_utils.ino"
void controller_startup();
#line 17 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
void ledLoop(void *pvParameters);
#line 51 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
void ledMatrixSetup();
#line 90 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
void displayCb(void *pvParameters);
#line 116 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
void wiFiConnectHelper();
#line 156 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
void storeSsidPass(void *pvParameters);
#line 173 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
void ledMatrixRandomInit();
#line 182 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
void socketDataProcessor(WiFiClient &client);
#line 15 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\servo_utils.ino"
long get_pwm_from_speed(float speed);
#line 21 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\servo_utils.ino"
void startup_prints();
#line 32 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\servo_utils.ino"
void servo_init();
#line 45 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\servo_utils.ino"
void set_motor_speed(int channel , float speed);
#line 57 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\servo_utils.ino"
void fast_blink(int count);
#line 15 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\robot.ino"
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



#line 1 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\controller_utils.ino"
#include "headers/controller_utils.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS] = {nullptr,nullptr,nullptr,nullptr};

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle()     // (0 - 1023): throttle (AKA gas) button
    );
}


void processGamepad(ControllerPtr ctl) {
    getLeftStickVector(ctl);
}


void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

void controller_startup() {

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.enableVirtualDevice(false);
}

BLA::Matrix<1, 2, float> getLeftStickVector(ControllerPtr ctl){
    int32_t x = ctl->axisX();        // (-511 - 512) left X Axis
    int32_t y = -ctl->axisY();

    BLA::Matrix<1,2,float> b;
    b.Fill(0);
    float mag = 512.0f;

    if (x == 0 and y == 0){
        return b;
    }

    b(0,0) = x/mag;
    b(0,1) = y/mag;
    
    return b;
}
#line 1 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\led.ino"
#include "headers/led.h"

uint8_t led_data[50*DATA_LEN];
CRGB leds[NUM_LEDS];
uint16_t total_frames=1;
uint16_t fps =1;
uint16_t brightness =10;
uint8_t color_correction[3] = {255,255,255}; 

//Wifi Config
IPAddress local_IP(192, 168, 0, 201); // Set your desired IP
IPAddress gateway(192, 168, 0, 1);    // Gateway IP (usually your router's IP)
IPAddress subnet(255, 255, 255, 0); 

TaskHandle_t displayTask;

void ledLoop(void *pvParameters){

    const int serverPort = 12345;
    WiFiServer server(serverPort);

    wiFiConnectHelper();

    // Start the server
    server.begin();
    Serial.println("TCP server started!");

    WiFiClient client;

    while(true){

        client = server.available();
        if (client) {
            Serial.println("New client connected!");

            while (client.connected()) {
                if (client.available()) {
                  
                    socketDataProcessor(client);
                    
                }
                vTaskDelay(50/portTICK_PERIOD_MS);
            }
            Serial.println("Client disconnected.");
            client.stop();
            vTaskDelay(500/portTICK_PERIOD_MS);
        }
    }
    return;
}
void ledMatrixSetup(){
    FastLED.clear();
    ledMatrixRandomInit();
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.setCorrection(TypicalSMD5050);
    FastLED.show();

    EEPROM.begin(EEPROM_SIZE);

    xTaskCreatePinnedToCore(
        ledLoop,          // Task function
        "ledLoop",        // Task name
        2048<<4,               // Stack size in words
        NULL,               // Parameter passed to the task
        1,                  // Task priority
        NULL,               // Task handle
        1                   // Core ID
    );
    xTaskCreatePinnedToCore(
        displayCb,          // Task function
        "displayCb",        // Task name
        2048,               // Stack size in words
        NULL,               // Parameter passed to the task
        2,                  // Task priority
        &displayTask,               // Task handle
        1                   // Core ID
    );

    xTaskCreatePinnedToCore(
        storeSsidPass,          // Task function
        "ssidpass",        // Task name
        2048,               // Stack size in words
        NULL,               // Parameter passed to the task
        1,                  // Task priority
        NULL,               // Task handle
        1                   // Core ID
    );
}
void displayCb(void *pvParameters)
{

    int stride0 = 192;
    int stride1 = 24;
    int stride2 = 3;
    int frame=0;

    while(true){
        for(int x = 0 ; x < MATRIX_HEIGHT ; x++){
            for(int y = 0 ; y < MATRIX_WIDTH ; y++){
                uint8_t ir = led_data[ frame*stride0 + x*stride1 + y*stride2 + 0];
                uint8_t ig = led_data[ frame*stride0 + x*stride1 + y*stride2 + 1];
                uint8_t ib = led_data[ frame*stride0 + x*stride1 + y*stride2 + 2];
                CRGB color = CRGB(ir,ig,ib);
                leds[(MATRIX_HEIGHT-1-x)*8 + (MATRIX_WIDTH-1-y)] = color;;
            }
        }
        FastLED.show();
        
        frame++;
        vTaskDelay((1000/fps)/portTICK_PERIOD_MS);
        if(frame >= total_frames) frame=0;
    }
    return;
}
void wiFiConnectHelper(){
    

    std::array<String,2> id_pass = getSsidPass();
    
    Serial.println(id_pass[0]);
    const char * ssid = id_pass[0].c_str();
    const char * password = id_pass[1].c_str();
    
    WiFi.config(local_IP,gateway,subnet);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi ");
    Serial.print(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi!");
    Serial.println(WiFi.localIP());
}
std::array<String,2> getSsidPass(){

    char ssid_pass[64];
    EEPROM.readBytes(0,(void*)ssid_pass,sizeof(ssid_pass));

    int i = 0;
    String ssid;

    for(i = 0 ; ssid_pass[i]!=' ';i++){
        ssid+=ssid_pass[i];
    }

    String pass;
    for(i = i+1 ; ssid_pass[i]!='\0';i++){
        pass+=ssid_pass[i];
    }

    return std::array<String,2>({ssid,pass});
}

void storeSsidPass(void *pvParameters){
    
    while(true){
        if(Serial.available()){
            String data = Serial.readStringUntil('\0');
            if(data != NULL) {
                int i = 0;
                for(i = 0 ; data[i] != '\0';i++){
                    EEPROM.put(i,data[i]);
                }
                EEPROM.put(i,'\0');
                EEPROM.commit();
                Serial.println("SSID PASS saved. Please Restart.\n"+ data);
            }
        }
    }
}
void ledMatrixRandomInit(){

    for(int i = 0 ; i < DATA_LEN ; i+=3){
        led_data[i] = random(0,255);
        led_data[i+1] = random(0,255);
        led_data[i+2] = random(0,255);
        leds[i/3] = CRGB(led_data[i],led_data[i+1],led_data[i+2]);
    }
}
void socketDataProcessor(WiFiClient &client){

    size_t frame_len =client.readBytes((char *)&total_frames,sizeof(total_frames));
    size_t fps_len =client.readBytes((char *)&fps,sizeof(fps));
    size_t brightness_len =client.readBytes((char *)&brightness,sizeof(brightness));
    size_t correc_len =client.readBytes((char *)&color_correction,sizeof(color_correction));

    if(total_frames > 50 or total_frames < 1){
        client.print("Led Data error");
        return;
    }
    if(fps < 1 or fps > 60){

        client.print("Led Data error");
        return;
    }
    if( brightness < 0 or brightness > 255){

        client.print("Led Data error");
        return;
    }
    if(frame_len != sizeof(total_frames) or fps_len != sizeof(fps) or brightness_len != sizeof(brightness) or correc_len != sizeof(color_correction)){

        client.print("Led Data error");
        return;
    }

    Serial.print("Received: ");
    Serial.printf("frame %d fps %d brightness %d\n",total_frames,fps,brightness);

    size_t len = client.readBytes(led_data,total_frames*DATA_LEN);

    FastLED.setBrightness(brightness);
    FastLED.setCorrection(CRGB(color_correction[0],color_correction[1],color_correction[2]));
    
    client.print("Thanks paisi");
    return;
}
#line 1 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\motor_utils.ino"
#include "headers/motor_utils.h"

BLA::Matrix<2,3,float> vmatrix_pinv={
     0.47140452,  0.17254603, -0.64395055,
     -0.47140452,  0.64395055, -0.17254603
};
BLA::Matrix<1, 3, float> getCoefficents(){

    ControllerPtr myController = myControllers[0];

    if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                
                return coefficentCalculator();
            } else {
                Serial.println("Unsupported controller");
            }
    }
    return BLA::Zeros<1, 3, float>();
}

BLA::Matrix<1, 3, float> coefficentCalculator(){

    ControllerPtr myController = myControllers[0];
    BLA::Matrix<1, 3, float> coeff;
    coeff.Fill(0.0);
    if(myController->brake() > 20){
        float coeff_val = myController->brake() / -1024.0;
        coeff.Fill(coeff_val);
        return coeff;
    }
    if(myController->throttle() > 20){
        float coeff_val = myController->throttle() / 1024.0;
        coeff.Fill(coeff_val);
        return coeff;
    }
    coeff = getLeftStickVector(myControllers[0]) * vmatrix_pinv;
    return coeff;
}
#line 1 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\servo_utils.ino"
#include "headers/servo_utils.h"



const int time_period = (int)( (1.0/SERVO_FREQ) * 1e6);
const int pwm_min_width = map(REVERSE_PULSE_WIDTH , 0 , time_period , 0 , 4096);
const int pwm_max_width = map(FORWARD_PULSE_WIDTH , 0 , time_period , 0 , 4096);
int pwm_diff = pwm_max_width - pwm_min_width;


PCA9685 pwmController(B000000);  
PCA9685_ServoEval pwmServo(pwm_min_width,pwm_max_width);


long get_pwm_from_speed(float speed){
  
  float val = (speed+1.0f)/2.0f;
  float pwm = pwm_min_width + (pwm_diff*val);
  return (long)pwm;
}
void startup_prints(){
  Serial.print("Time period: " );
  Serial.print(time_period  );
  Serial.print(" Min and Max Widths: ");
  Serial.print(pwm_min_width);
  Serial.print(" ");
  Serial.println(pwm_max_width);
  return;
}


void servo_init(){

  fast_blink(11); //use odd numbers
          
  Wire.begin();
  pwmController.resetDevices();      
  pwmController.init();               
  pwmController.setPWMFrequency(SERVO_FREQ); 

  fast_blink(11); //use odd numbers
  return;
}

void set_motor_speed(int channel , float speed){

    static long prev_pwm[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    long pwm = get_pwm_from_speed(speed);

    long shift = 10;
    long new_pwm = (( prev_pwm[channel] * ((1<<shift)-1) )>>shift) + (pwm>>shift);
    pwmController.setChannelPWM(channel, new_pwm);
    prev_pwm[channel] = pwm;
    return;
}

void fast_blink(int count){
  for(int i = 0 ; i < count ; i++){
    digitalWrite(LED_BUILTIN , !digitalRead(LED_BUILTIN));
    delay(FAST_BLINK_DELAY);
  }
  return;
}


