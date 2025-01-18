/*
 * rosserial Subscriber Example
 * Blinks an LED on callback
 */
// #undef ESP8266
// #undef ESP32

#include<WiFi.h>
#include <ros.h>
#include <std_msgs/Empty.h>
#include <std_msgs/UInt8MultiArray.h>
#include <FastLED.h>

#define SSID "drazam_plus"
#define password "fallguys"
#define LED_PIN 5
#define MATRIX_WIDTH  8   // Width of the matrix
#define MATRIX_HEIGHT 8   // Height of the matrix
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

// LED array
CRGB leds[NUM_LEDS];

// ros variables
ros::NodeHandle nh;
IPAddress server(192, 168, 0, 100);  // Replace with ROS host IP
const uint16_t serverPort = 11411;  // Default rosserial TCP port

int getIndex(int x, int y) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) return -1;
  return (y % 2 == 0) ? (y * MATRIX_WIDTH + x) : (y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x));
}
void drawPixel(int x, int y, CRGB color) {
  int index = getIndex(x, y);
  if (index >= 0 && index < NUM_LEDS) {
    leds[index] = color;
  }
  return;
}
void displayCb(const std_msgs::UInt8MultiArray &display_matrix)
{
    static int count =0;
    Serial.print(count++);
    digitalWrite(2, HIGH - digitalRead(2)); // blink the led

    int stride0 = display_matrix.layout.dim[0].stride;
    int stride1 = display_matrix.layout.dim[1].stride;
    int stride2 = display_matrix.layout.dim[2].stride;

    for(int x = 0 ; x < MATRIX_HEIGHT ; x++){
        for(int y = 0 ; y < MATRIX_WIDTH ; y++){
            uint8_t ir = display_matrix.data[x*stride0 + y*stride1 + 0];
            uint8_t ig = display_matrix.data[x*stride0 + y*stride1 + 1];
            uint8_t ib = display_matrix.data[x*stride0 + y*stride1 + 2];
            CRGB color = CRGB(ir,ig,ib);
            leds[x*8 + y] = color;;
        }
    }
    FastLED.show();
    return;
}

ros::Subscriber<std_msgs::UInt8MultiArray> sub("display_matrix", &displayCb);

void setup()
{
    Serial.begin(57600);
    WiFi.begin(SSID,password);


    pinMode(2, OUTPUT);
    nh.getHardware()->setConnection(server,serverPort);
    nh.initNode();
    nh.subscribe(sub);


    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(100);
    FastLED.clear();
    FastLED.show();
}

void loop()
{
    nh.spinOnce();
    delay(1);
}