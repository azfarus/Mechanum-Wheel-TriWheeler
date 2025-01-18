#include <WiFi.h>
#include<FastLED.h>

// Wi-Fi credentials
#define LED_PIN 5
#define MATRIX_WIDTH  8   // Width of the matrix
#define MATRIX_HEIGHT 8   // Height of the matrix
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define DATA_LEN 64*3

// LED array
CRGB leds[NUM_LEDS];


const char* ssid = "drazam_plus";
const char* password = "fallguys";
uint8_t led_data[DATA_LEN];

// Server settings
const int serverPort = 12345;
WiFiServer server(serverPort);

void displayCb();
void ledMatrixSetup(){
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(100);
    FastLED.clear();
    FastLED.show();
}
void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.println("TCP server started!");

  //setup the led matrix
  ledMatrixSetup();
}

void loop() {
  // Check for client connection
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected!");

    while (client.connected()) {
      if (client.available()) {
        size_t len = client.readBytes(led_data,DATA_LEN);
        Serial.println("Received: ");

        // Echo the message back
        displayCb();
        client.println("Displayed");
      }
    }
    Serial.println("Client disconnected.");
    client.stop();
  }
}



void displayCb()
{
    static int count =0;
    Serial.print(count++);
    digitalWrite(2, HIGH - digitalRead(2)); // blink the led

    int stride0 = 24;
    int stride1 = 3;


    for(int x = 0 ; x < MATRIX_HEIGHT ; x++){
        for(int y = 0 ; y < MATRIX_WIDTH ; y++){
            uint8_t ir = led_data[x*stride0 + y*stride1 + 0];
            uint8_t ig = led_data[x*stride0 + y*stride1 + 1];
            uint8_t ib = led_data[x*stride0 + y*stride1 + 2];
            CRGB color = CRGB(ir,ig,ib);
            leds[x*8 + y] = color;;
        }
    }
    FastLED.show();
    return;
}