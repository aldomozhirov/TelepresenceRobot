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

Motor_Control motor;

void setMotorsSpeed(double speed) {
	motor.SetSpeed(motor.PWM_3_pin, speed);
	motor.SetSpeed(motor.PWM_9_pin, speed);
	motor.SetSpeed(motor.PWM_5_pin, speed);
	motor.SetSpeed(motor.PWM_6_pin, speed);
}

int main()
{
       signal(SIGINT, &sig_handler);

       cout << "[rc_car] Application Started" << endl;
	   
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
					setMotorsSpeed(SPEED3);
                    motor.run(Motor3, FORWARD);
                    motor.run(Motor4, FORWARD);
                    break;
             case 'x'://REVERSE
					setMotorsSpeed(SPEED3);
                    motor.run(Motor3, BACKWARD);
                    motor.run(Motor4, BACKWARD);
                    break;
             case 'a'://TURN LEFT
					setMotorsSpeed(SPEED1);
                    motor.run(Motor3, FORWARD);
                    motor.run(Motor4, BACKWARD);
                    break;
             case 'd'://TURN RIGHT
					setMotorsSpeed(SPEED1);
                    motor.run(Motor3, BACKWARD);
                    motor.run(Motor4, FORWARD);
                    break;
             case 'q'://ROTARY MOVE LEFT FORWARD
					setMotorsSpeed(SPEED1);
                    motor.run(Motor3, FORWARD);
                    motor.run(Motor4, STOP);
                    break;
             case 'z'://ROTARY MOVE LEFT BACKWARD
					setMotorsSpeed(SPEED1);
                    motor.run(Motor3, BACKWARD);
                    motor.run(Motor4, STOP);
                    break;
             case 'e'://ROTARY MOVE RIGHT FORWARD
					setMotorsSpeed(SPEED1);
                    motor.run(Motor3, STOP);
                    motor.run(Motor4, FORWARD);
                    break;
             case 'c'://ROTARY MOVE RIGHT BACKWARD
					setMotorsSpeed(SPEED1);
                    motor.run(Motor3, STOP);
                    motor.run(Motor4, BACKWARD);
                    break;
             case '1'://SET SPEED 100%
					//deprecated
                    break;
             case '2'://SET SPEED 90%
					//deprecated
                    break;
             case '3'://SET SPEED 80%
					//deprecated
                    break;
             case '4'://SET SPEED 70%
					//deprecated
                    break;
             case 's'://STOP
                    motor.run(Motor3, STOP);
                    motor.run(Motor4, STOP);
                    break;
             case 'g':// CLOSE APPLICATION
                    return 0;
                    break;
             }
             fclose(fp);
             close(fd); 
       }
       motor.standby(OFF);
       cout << "[rc_car] Closing application..." << endl;
       sleep(1);
       return MRAA_SUCCESS;
}

void sig_handler(int signum)
{     
       if (signum == SIGINT) isrunning = 0;
}