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

