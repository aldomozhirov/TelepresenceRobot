/*     ---------------------------------------------------------
*     |  Intel Edison C++ Example Code                         |
*     |   PWM EXAMPLE                                         |
*     ---------------------------------------------------------
* the motor shield with Intel Edison by SSH
* Makefile: g++ -lmraa –o MOTOR MOTOR.cpp
* ./MOTOR
*/

#ifndef Motor_Shield_h
#define Motor_Shield_h

#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <mraa.h>
#include <mraa/pwm.h>
#include <mraa/gpio.h>

//Pin Definitions
//74HC595 pins of the Motor Shield V1
#define DIR_CLOCK   4
#define DIR_EN             7
#define DIR_SER            8
#define DIR_LATCH   12
//74HC595 pins of the Motor Shield V1
#define PWM0A       6 //L293D IC2 1-2EN
#define PWM0B       5 //L293D IC2 3-4EN
//Servo pins of the Motor Shield V1
#define PWM1A       9 //SERVO_1
#define PWM1B       10//SERVO_2
//74HC595 pins of the Motor Shield V1
#define PWM2A       11//L293D IC1 1-2EN
#define PWM2B       3 //L293D IC1 3-4EN

//Special commands
#define LOW  0
#define HIGH 1
#define LSBFIRST 0
#define MSBFIRST 1
#define ON 1
#define OFF 0
#define OK 0
#define NOK 1

// Bit positions in the 74HCT595 shift register output
#define MOTOR1_A 2//2 //QC
#define MOTOR1_B 3//3 //QD
#define MOTOR2_A 1//1 //QB
#define MOTOR2_B 4//4 //QE
#define MOTOR4_A 0//0 //QF
#define MOTOR4_B 6//6 //QH
#define MOTOR3_A 5//5 //QA
#define MOTOR3_B 7//7 //QG

// Constants that the user passes in to the motor calls
#define FORWARD  1
#define BACKWARD 2
#define STOP 3

#define Motor1   1
#define Motor2   2
#define Motor3   3
#define Motor4   4

#define SPEED1   1
#define SPEED2   0.9
#define SPEED3   0.8
#define SPEED4   0.7

using namespace std;

static uint8_t latch_state=0;//GLobal variable

class Motor_Control
{
public:
      
       // No parameters need to be passed to the constructor.
       Motor_Control();
       mraa_pwm_context PWM_3_pin;
       mraa_pwm_context PWM_5_pin;
       mraa_pwm_context PWM_6_pin;
       mraa_pwm_context PWM_9_pin;
       mraa_gpio_context GPIO_10_pin;
       mraa_gpio_context GPIO_11_pin;
       int SetSpeed(mraa_pwm_context, float);
       int SetSpeed(mraa_gpio_context, bool);
       void run(uint8_t , uint8_t );
       void standby(bool );
private:
       mraa_gpio_context data_pin;
       mraa_gpio_context clock_pin;
       mraa_gpio_context latch_pin;
       mraa_gpio_context enable_pin;

       void shiftOut(mraa_gpio_context dataPin, mraa_gpio_context clockPin, uint8_t bitOrder, uint8_t val);
       void _74HC595();
       int check_Initialization();
       void errors(int);
};

#endif // !rc_car