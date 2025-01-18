#line 1 "G:\\MyProjects\\TriangularRobot\\Arduino\\robot\\headers\\servo_utils.h"

#include <PCA9685.h>


#define FAST_BLINK_DELAY 200
#define SERVO_FREQ 50
#define REVERSE_PULSE_WIDTH 600
#define FORWARD_PULSE_WIDTH 2600
#define LED_BUILTIN 2

void startup_prints();
void fast_blink(int count);
void servo_init();
long get_pwm_from_speed(int speed);
void set_motor_speed(int channel , float speed);

