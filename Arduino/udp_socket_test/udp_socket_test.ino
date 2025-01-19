#include <WiFi.h>

// Wi-Fi credentials
#define LED_PIN 5
#define MATRIX_WIDTH  8   // Width of the matrix
#define MATRIX_HEIGHT 8   // Height of the matrix
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define DATA_LEN 64*3

uint16_t total_frames=1;
uint16_t fps =1;
uint16_t brightness =10;
uint8_t color_correction[3] = {255,255,255}; 
uint8_t led_data[50*DATA_LEN];



const char* ssid = "drazam_plus";
const char* password = "fallguys";

// Server settings
const int serverPort = 12345;
WiFiServer server(serverPort);

void displayCb();
void socketDataProcessor();

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
        socketDataProcessor();
        client.println("Thanks Paisi");
      }
    }
    Serial.println("Client disconnected.");
    client.stop();
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

    for(int i = 0 ; i < sizeof(led_data); i++){
      Serial.print(i);
      Serial.print(' ');
    }
    Serial.println();
    
    client.print("Thanks paisi");
    return;
}