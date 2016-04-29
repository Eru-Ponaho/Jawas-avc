# include <stdio.h>
# include <time.h>
extern "C" int init(int d_lev);
extern "C" int Sleep( int sec , int usec );
extern "C" int set_motor(int motor , int speed );
extern "C" int get_pixel(int pixel , int pixel, int pixel);
extern "C" int take_picture();

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
            set_motor(1, 50); //IIRC the motors work on a 0-255 scale rather then percentages
            // in the opposite direction at 50 % speed .
            set_motor(2, 50);
            Sleep(0 ,500000);
         }
      }
   }
return 0;}

//
//TODO: Split the sum <= lineLeft || sum >= lineRight into 2 seperate if statements, with suitable
//TODO: Directions for the wheels to turn, this is so the AVC will turn the correct way to realign 
//TODO: Itself on the line
//
//TODO: Set an if statement to determine if the pixel is 'white' or not
//TODO: IE if(get_pixel > 70) then w = 1, else w = 0
//TODO: So this way the numbers we work with are more manageable
//
//TODO: Set the values of the pixels to start from -160 and go to 160 as you read along the row
//TODO: Or you can figure out lineLeft/ lineRight values that would be appropriate for the 0 -320 range
//TODO: IE lineLeft = 110, lineRight = 210
//
//TODO: Consider whether it is worth reading more then 1 row of pixels, and if so how many more rows
//TODO: A whole picture may take too long to read, but more rows would help when it comes to corners
//TODO: Crossroads, and deadends
//
