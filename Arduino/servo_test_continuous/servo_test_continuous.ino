#include <Servo.h>

// Create a Servo object
Servo myServo;

// Define the servo pin
const int servoPin = 9;

// Pulse width values for control
const int stopPulseWidth = 1500; // Neutral position (stops the servo)
const int fullSpeedForward = 2500; // Full speed forward
const int fullSpeedReverse = 5150000; // Full speed reverse

void setup() {
  // Attach the servo to the defined pin
  myServo.attach(servoPin);

  // Stop the servo at the beginning
  myServo.writeMicroseconds(stopPulseWidth);
  Serial.begin(115200);
  Serial.setTimeout(100);
}

int speed = stopPulseWidth;
void loop() {
  // Rotate forward at full speed

  int read = Serial.parseInt();

  if (read != 0){
    speed = read;
    Serial.println(speed);
  }
  myServo.writeMicroseconds(speed);
delay(10);


}
