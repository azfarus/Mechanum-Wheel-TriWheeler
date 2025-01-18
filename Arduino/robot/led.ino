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