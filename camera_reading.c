# include <stdio.h>
# include <time.h>
extern "C" int init(int d_lev);
extern "C" int Sleep( int sec , int usec );
extern "C" int set_motor(int motor , int speed );
extern "C" int get_pixel(int pixel , int pixel, int pixel);

int main (){
   // This sets up the RPi hardware and ensures
   // everything is working correctly
   init(0);
   // Sets the motor connected to pin 1 to rotate
   // in one direction at MAX speed .
   while (true){
     take_picture();
      int sum = 0;
      int i;
      int lineLeft = -54;
      int lineRight = 54;
      for(i=0, i<320, i++){
         w = get_pixel(120,i,3);
         sum = sum + i * w ;
         if (sum <= lineLeft || sum >= lineRight){
            set_motor(1, 50);
            // in the opposite direction at 50 % speed .
            set_motor(2, 50);
            Sleep(0 ,500000);
         }
      }
   }
return 0;}
