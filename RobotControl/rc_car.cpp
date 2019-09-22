/* 
* This code control the Adafruit Motor_Shield_V1 and Intel Edison
* that the have been assembly in a old rc car
*
* Makefile: g++ -pthread-lmraa –o rc_car rc_car.cpp Motor_Shield.cpp
* ./rc_car
*/
#include "Motor_Shield.h"
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 

using namespace std;

sig_atomic_t volatile isrunning = 1;
void sig_handler(int signum);

int main()
{
       signal(SIGINT, &sig_handler);

       Motor_Control motor;

       cout << "Press any 'g' and then Enter to exit from the aplication..." << endl;
	   
       char * myfifo = "/tmp/myfifo"; 
       mkfifo(myfifo, 0666); 
       char buffer[80]; 
	   
       while (isrunning)
       {
             int fd = open(myfifo,O_RDONLY); 
             FILE *fp = fdopen(fd, "r");
             fgets(buffer, 80, fp);	 
             char c = buffer[0];
             switch (c)
             {
             case 'w'://STRAIGHT
					motor.run(Motor1, FORWARD);
                    motor.run(Motor2, FORWARD);
                    motor.run(Motor3, FORWARD);
                    motor.run(Motor4, FORWARD);
                    break;
             case 's'://REVERSE
					motor.run(Motor1, BACKWARD);
                    motor.run(Motor2, BACKWARD);
                    motor.run(Motor3, BACKWARD);
                    motor.run(Motor4, BACKWARD);
                    break;
             case 'a'://TURN LEFT
					motor.run(Motor1, BACKWARD);
                    motor.run(Motor2, FORWARD);
                    motor.run(Motor3, FORWARD);
                    motor.run(Motor4, BACKWARD);
                    break;
             case 'd'://TURN RIGHT
					motor.run(Motor1, FORWARD);
                    motor.run(Motor2, BACKWARD);
                    motor.run(Motor3, BACKWARD);
                    motor.run(Motor4, FORWARD);
                    break;
             case 'q'://ROTARY MOVE LEFT FORWARD
					motor.run(Motor1, STOP);
                    motor.run(Motor2, FORWARD);
                    motor.run(Motor3, FORWARD);
                    motor.run(Motor4, STOP);
                    break;
             case 'e'://ROTARY MOVE LEFT BACKWARD
					motor.run(Motor1, STOP);
                    motor.run(Motor2, BACKWARD);
                    motor.run(Motor3, BACKWARD);
                    motor.run(Motor4, STOP);
                    break;
             case 'z'://ROTARY MOVE RIGHT FORWARD
					motor.run(Motor1, FORWARD);
                    motor.run(Motor2, STOP);
                    motor.run(Motor3, STOP);
                    motor.run(Motor4, FORWARD);
                    break;
             case 'c'://ROTARY MOVE RIGHT BACKWARD
					motor.run(Motor1, BACKWARD);
                    motor.run(Motor2, STOP);
                    motor.run(Motor3, STOP);
                    motor.run(Motor4, BACKWARD);
                    break;
             case '1'://SET SPEED 100%
					motor.SetSpeed(motor.PWM_3_pin, SPEED1);
					motor.SetSpeed(motor.PWM_9_pin, SPEED1);
                    motor.SetSpeed(motor.PWM_5_pin, SPEED1);
                    motor.SetSpeed(motor.PWM_6_pin, SPEED1);
                    break;
             case '2'://SET SPEED 90%
					motor.SetSpeed(motor.PWM_3_pin, SPEED2);
					motor.SetSpeed(motor.PWM_9_pin, SPEED2);
                    motor.SetSpeed(motor.PWM_5_pin, SPEED2);
                    motor.SetSpeed(motor.PWM_6_pin, SPEED2);
                    break;
             case '3'://SET SPEED 80%
					motor.SetSpeed(motor.PWM_3_pin, SPEED3);
					motor.SetSpeed(motor.PWM_9_pin, SPEED3);
                    motor.SetSpeed(motor.PWM_5_pin, SPEED3);
                    motor.SetSpeed(motor.PWM_6_pin, SPEED3);
                    break;
             case '4'://SET SPEED 70%
					motor.SetSpeed(motor.PWM_3_pin, SPEED4);
					motor.SetSpeed(motor.PWM_9_pin, SPEED4);
                    motor.SetSpeed(motor.PWM_5_pin, SPEED4);
                    motor.SetSpeed(motor.PWM_6_pin, SPEED4);
                    break;
             case ' '://STOP
					motor.run(Motor1, STOP);
                    motor.run(Motor2, STOP);
                    motor.run(Motor3, STOP);
                    motor.run(Motor4, STOP);
                    break;
             case 'g':
                    return 0;
                    break;
             }
             fclose(fp);
             close(fd); 
       }
       motor.standby(OFF);
       cout << "\nClosing application..." << endl;
       sleep(1);
       return MRAA_SUCCESS;
}

void sig_handler(int signum)
{     
       if (signum == SIGINT) isrunning = 0;
}