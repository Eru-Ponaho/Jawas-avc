
# include <stdio.h>
# include <time.h>
extern "C" int init(int d_lev);
extern "C" int Sleep( int sec , int usec );
extern "C" int set_motor(int motor , int speed );
int main (){
    while (t < i){
     // This sets up the RPi hardware and ensures
     // everything is working correctly
     init(0);
     // Sets the motor connected to pin 1 to rotate
     // in one direction at a low speed .
     set_motor(1, 20);
     // Sets the motor connected to pin 2 to rotate
     // in the same direction at same speed .
     set_motor(2, 20);
     Sleep(0 ,500000);
     i++;
     }
return 0;}
