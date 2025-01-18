#include<FastLED.h>
#include<WiFi.h>
#include <EEPROM.h>
#include<array>

#define LED_PIN 5
#define MATRIX_WIDTH  8   // Width of the matrix
#define MATRIX_HEIGHT 8   // Height of the matrix
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define DATA_LEN 64*3
#define LED_BRIGHTNESS 10
#define EEPROM_SIZE 64



void displayCb(void *pvParameters);
void ledLoop(void *pvParameters);
void storeSsidPass(void *pvParameters);

void ledMatrixSetup();
void ledMatrixRandomInit();
void socketDataProcessor(WiFiClient &client);
void wiFiConnectHelper();
std::array<String,2> getSsidPass();
