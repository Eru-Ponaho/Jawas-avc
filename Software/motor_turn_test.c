# include <stdio.h>
# include <time.h>
extern "C" int init(int d_lev);
extern "C" int Sleep( int sec , int usec );
extern "C" int set_motor(int motor , int speed );
int main (){
   // This sets up the RPi hardware and ensures
   // everything is working correctly
   init(0);                         
   
   // Sets the motor connected to pin 1 to rotate
   // in one direction at MAX speed .
   set_motor(1, 255); 
   //SetMotor(1, 1, 255);    Old code from before library was changed
   Sleep (0 ,500000);
   
   // Sets the motor connected to pin 0 to rotate
   // in the opposite direction at MAX speed .
   set_motor(2, -127);            
   //SetMotor(0, 2, 255)    Old code from before library was changed
   Sleep(0 ,500000);
   
return 0;}
