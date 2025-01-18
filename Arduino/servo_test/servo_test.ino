
#include <PCA9685.h>


#define FAST_BLINK_DELAY 200
#define TOPIC "joint_positions"
#define SERVO_ANGLE_OFFSET 0 //Ros gives angles from -90 to 90 degrees but MCU works with 0 to 180 degrees, thus add 90deg offset
#define SERVO_FREQ 50
#define REVERSE_PULSE_WIDTH 600
#define FORWARD_PULSE_WIDTH 2600
#define LED_BUILTIN 2

void startup_prints();
void fast_blink(int count);
void servo_init();
int get_pwm_from_speed(int speed);

const int time_period = (int)( (1.0/SERVO_FREQ) * 1e6);
const int pwm_min_width = map(REVERSE_PULSE_WIDTH , 0 , time_period , 0 , 4096);
const int pwm_max_width = map(FORWARD_PULSE_WIDTH , 0 , time_period , 0 , 4096);
int pwm_diff = pwm_max_width - pwm_min_width;


PCA9685 pwmController(B000000);  
PCA9685_ServoEval pwmServo(pwm_min_width,pwm_max_width);


void setup(){


    // ros and blinkled initialization
    pinMode(LED_BUILTIN,OUTPUT);
    servo_init();
    startup_prints();

}


int i=0;
void loop(){

  int read = Serial.parseInt();
  if(!(read==0)){
    i = read;
  }
  pwmController.setChannelPWM(0, i);
  pwmController.setChannelPWM(1, i);
  pwmController.setChannelPWM(2, i);
  // pwmController.setChannelPWM(1, 512);
  // pwmController.setChannelPWM(2, 307);
}


int get_pwm_from_speed(int speed){

  int val = (speed+100);
  long pwm = pwm_min_width + (pwm_diff*val)/200 + 1;
  return pwm;
}
void startup_prints(){
  Serial.print("Time period: " );
  Serial.print(time_period  );
  Serial.print(" Min and Max Widths: ");
  Serial.print(pwm_min_width);
  Serial.print(" ");
  Serial.println(pwm_max_width);
}


void servo_init(){

  fast_blink(11); //use odd numbers

  Serial.begin(115200);    
  Serial.setTimeout(100);           
  Wire.begin();

  pwmController.resetDevices();      
  pwmController.init();               
  pwmController.setPWMFrequency(SERVO_FREQ); 

  fast_blink(11); //use odd numbers
}



void fast_blink(int count){
  for(int i = 0 ; i < count ; i++){
    digitalWrite(LED_BUILTIN , !digitalRead(LED_BUILTIN));
    delay(FAST_BLINK_DELAY);
  }
}