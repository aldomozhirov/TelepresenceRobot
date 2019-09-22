#include "Motor_Shield.h"

/* Motor_Control()--> Constructor
 *
 */
Motor_Control::Motor_Control()
{
       mraa_init();

       //Initializing GPIO pins of the 74HC595
       data_pin = mraa_gpio_init(DIR_SER);
       clock_pin = mraa_gpio_init(DIR_CLOCK);
       latch_pin = mraa_gpio_init(DIR_LATCH);
       enable_pin = mraa_gpio_init(DIR_EN);

       mraa_gpio_dir(data_pin, MRAA_GPIO_OUT);
       mraa_gpio_dir(clock_pin, MRAA_GPIO_OUT);
       mraa_gpio_dir(latch_pin, MRAA_GPIO_OUT);
       mraa_gpio_dir(enable_pin, MRAA_GPIO_OUT);

       //Initializing PWM pins
       PWM_3_pin = mraa_pwm_init(PWM2B);
       PWM_5_pin = mraa_pwm_init(PWM0B);
       PWM_6_pin = mraa_pwm_init(PWM0A);
       PWM_9_pin = mraa_pwm_init(PWM1A);

       mraa_pwm_period_us(PWM_3_pin, 100);//period 100us
       mraa_pwm_period_us(PWM_5_pin, 100);//period 100us
       mraa_pwm_period_us(PWM_6_pin, 100);//period 100us
       mraa_pwm_period_us(PWM_9_pin, 100);//period 100us

       SetSpeed(PWM_3_pin,1);
       SetSpeed(PWM_5_pin,0.8);
       SetSpeed(PWM_6_pin,0.8);
       SetSpeed(PWM_9_pin,1);

       //Initializing GPIO pins of the Motor Shield which PWM cannnot be configure
       GPIO_10_pin = mraa_gpio_init(PWM1B);
       GPIO_11_pin = mraa_gpio_init(PWM2A);

       mraa_gpio_dir(GPIO_10_pin, MRAA_GPIO_OUT);
       mraa_gpio_dir(GPIO_11_pin, MRAA_GPIO_OUT);

       SetSpeed(GPIO_10_pin, ON);
       SetSpeed(GPIO_11_pin, ON);
       errors(check_Initialization());
      
       mraa_gpio_write(latch_pin, LOW);     //Pulls the chips latch low  
       shiftOut(data_pin, clock_pin, MSBFIRST, 0 >> 8); //Shifts out the 8 bits to the shift registe
       mraa_gpio_write(latch_pin, HIGH); //Pulls the latch high displaying the data

       // The constructor disables the outputs of the PWM by asserting the standby
       //  pin on the controller. You *must* use the standby() function to enable
       //  them before proceeding!
       standby(ON);//Disable Motor Shield
}

/* check_Initialization()
 * This function checks every pin initializated before. If there is a problem,
 * it will return an integer attached to the last problem detected
 * Motor_Control.check_Initialization();
 */
int Motor_Control::check_Initialization()
{
       int error = 0;//If Error=0, there are any problems.Otherwise, something has happened in the initilization
      
       if (PWM_3_pin == NULL)
             error = 3;
       if (clock_pin == NULL)
             error = 4;
       if (PWM_5_pin == NULL)
             error = 5;
       if (PWM_6_pin == NULL)
             error = 6;
       if (enable_pin == NULL)
             error = 7;
       if (data_pin == NULL)
             error = 8;
       if (PWM_9_pin == NULL)
             error = 9;
       if (GPIO_10_pin == NULL)
             error = 10;
       if (GPIO_11_pin == NULL)
             error = 11;
       if (latch_pin == NULL)
             error = 12;
       return error;
}

/* errors()
 * List of mesages of posible errors
 * Motor_Control.errors();
 */
void Motor_Control::errors(int error)
{
       switch(error)
       {
             case 0:
                    cout << "OK... \n";
                    break;
             case 1:
                    cout << "NOK!!! \n";
                    break;
             case 2:

                    break;
             case 3:
                    cout << "Error in PWM_3_pin (PWM2B)=IC1 1-2EN...\n";
                    break;
             case 4:
                    cout << "Error in clock_pin 74HC595...\n";
                    break;
             case 5:
                    cout << "Error in PWM_5_pin (PWM0B)=IC2 1-2EN...\n";
                    break;
             case 6:
                    cout << "Error in PWM_6_pin (PWM0A))=IC2 3-4EN...\n";
                    break;
             case 7:
                    cout << "Error in enable_pin 74HC595...\n";
                    break;
             case 8:
                    cout << "Error in data_pin 74HC595...\n";
                    break;
             case 9:
                    cout << "Error in PWM_9_pin (PWM1A)=SERVO_2...\n";
                    break;
             case 10:
                    cout << "Error in GPIO_10_pin (PWM2B)=SERVO_1...\n";
                    break;
             case 11:
                    cout << "Error in GPIO_11_pin (PWM0B)=IC2 1-2EN...\n";
                    break;
             case 12:
                    cout << "Error in latch_pin 74HC595...\n";
                    break;
             case 13:
                    break;
             case 20:
                    cout << "Motor Speed is incorrect. Speed must be between 0.0 and 1.0\n";
                    break;
             case 21:
                    cout << "Motor Selection is incorrect. The motor number must be 1,2,3 or 4\n";
                    break;
             case 22:
                    cout << "Motor action is incorrect. The motor number must be 1,2 or 3 \n";
                    break;
             default:
                    break;
   
       }
}

/* stanby()
*  When stanby is OFF, the PWM signal worksn and the swift register is enabled
*  Otherwise, nothings will work.
*  Motor_bontrol.stanby(OFF) : PWM and 74HC595 work
*  Motor_bontrol.stanby(ON)  : PWM and 74HC595 don't work
*/
void Motor_Control::standby(bool OnOFF)
{
       mraa_pwm_enable(PWM_3_pin,OnOFF); //(PWM2B)=IC1 1-2EN

       mraa_pwm_enable(PWM_5_pin,OnOFF); //(PWM0B)=IC2 1-2EN

       mraa_pwm_enable(PWM_6_pin,OnOFF); //(PWM0A))=IC2 3-4EN

       mraa_pwm_enable(PWM_9_pin,OnOFF); //(PWM1A)=SERVO_2

       mraa_gpio_write(GPIO_10_pin, OnOFF); //(PWM2B)=SERVO_1

       mraa_gpio_write(GPIO_11_pin, OnOFF); //(PWM0B)=IC2 1-2EN =0

       mraa_gpio_write(enable_pin, 0); //It is enabled with a LOW volt level

}

/* speed(PWM);
 * This function configure the speed of the motors.
 * This sets the PWM duty between the 0% and 100%
 * Motor_Control.speed(PWM_3_pin,1.0)
 */
int Motor_Control::SetSpeed(mraa_pwm_context N_Motor,float duty)
{
       int error=0;
       if (duty > 1 || duty < 0)
       {
             duty = 0;
             error = 20;
       }

       if (duty == 0)
       {
             mraa_pwm_write(N_Motor, duty);
             mraa_pwm_enable(N_Motor, 0);
       }
       else
             mraa_pwm_enable(N_Motor, 1);
             error=mraa_pwm_write(N_Motor, duty);
       return error;
}

/* speed(GPIO);
* This function configure the speed of the motors
* Only has two speeds for pins 10 and 11.
*
* Motor_Control.speed(GPIO_11_pin,ON)
*/
int Motor_Control::SetSpeed(mraa_gpio_context N_Motor, bool OnOff)
{
       int error = 0;
       error=mraa_gpio_write(N_Motor, OnOff);
       return error;
}

void Motor_Control::run(uint8_t motornum, uint8_t action)
{
       uint8_t a, b,error;

       if (motornum > 4 || motornum < 0)
             error = 21;
       else
       {
             switch (motornum)
             {
             case 1:
                    a = 1 << MOTOR1_A; b = 1 << MOTOR1_B; break;
             case 2:
                    a = 1 << MOTOR2_A; b = 1 << MOTOR2_B; break;
             case 3:
                    a = 1 << MOTOR3_A; b = 1 << MOTOR3_B; break;
             case 4:
                    a = 1 << MOTOR4_A; b = 1 << MOTOR4_B; break;
             default:
                    return;
             }
       }

       if (action > 3 || action < 0)
             error = 22;
       else
       {
             switch (action)
             {
             case FORWARD:
                    latch_state |= a;//
                    latch_state &= ~b;
                    _74HC595();
                    break;
             case BACKWARD:
                    latch_state &= ~a;
                    latch_state |= b;
                    _74HC595();
                    break;
             case STOP:
                    latch_state &= ~a;     // A and B both low
                    latch_state &= ~b;
                    _74HC595();
                    break;
             }
       }
}

void Motor_Control::_74HC595()
{
       mraa_gpio_write(latch_pin, LOW);     //Pulls the chips latch low  
       shiftOut(data_pin, clock_pin, MSBFIRST, latch_state); //Shifts out the 8 bits to the shift register
       mraa_gpio_write(latch_pin, HIGH); //Pulls the latch high displaying the data
}


void Motor_Control::shiftOut(mraa_gpio_context dataPin, mraa_gpio_context clockPin, uint8_t bitOrder, uint8_t val)
{
       uint8_t i;

       for (i = 0; i < 8; i++) {
             if (bitOrder == LSBFIRST)
                    mraa_gpio_write(dataPin, !!(val & (1 << i)));
             else
                    mraa_gpio_write(dataPin, !!(val & (1 << (7 - i))));

             mraa_gpio_write(clockPin, HIGH);
             mraa_gpio_write(clockPin, LOW);
       }
}