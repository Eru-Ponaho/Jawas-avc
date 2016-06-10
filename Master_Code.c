#include <sys/time.h>
#include <stdio.h>

//#include <pthread.h>
#include <time.h>
#include <string.h>

// sudo gcc -Wall
extern "C" int init_hardware();
extern "C" int init(int d_lev);

extern "C" int take_picture();
extern "C" char get_pixel(int row, int col, int color);
extern "C" void set_pixel(int col, int row, char red,char green,char blue);

extern "C" int open_screen_stream();
extern "C" int close_screen_stream();
extern "C" int update_screen();
//extern "C" void GetLine(int row,int threshold);
extern "C" int display_picture(int delay_sec,int delay_usec);
extern "C" int save_picture(char filename[5]);

extern "C" int set_motor(int motor,int speed);

extern "C" int read_analog(int ch_adc);
extern "C" int Sleep(int sec, int usec);
extern "C" int select_IO(int chan, int direct);
extern "C" int write_digital(int chan,char level);
extern "C" int read_digital(int chan);
extern "C" int set_PWM(int chan, int value);

extern "C" int connect_to_server( char server_addr[15],int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);

int baseSpeed = 35; //with 40, can still complete B+ but osiclates heaviliy and doesnt seem reliable
int reverseSpeed = -45;
int LM = 0;
int RM = 0;
void crossRoad();
void deadEnd();
bool checkLine();
unsigned long getTime();
bool rotate = false;
int stuck = 0;
int threshold = 100;
unsigned long lastTime;
double lastError;
double derivativeSignal;
int turnSpeedStuck = 50;
int multiplier[320];
bool lineLeftSideVertical = false;
int left;//ir
int right;//ir
void turnLeft();
void turnRight();
int irError;
int irProportionalSignal;
int r;
int g;
int b;
bool wall = false;
float kp = 0.3;//proportionalSignalConstant


int main()
{
//===========================Gate Code===========================================

   //Open gate:
   //connects to server
    connect_to_server("130.195.6.196", 1024);
   //sends a message to the connected server
     send_to_server("Please");
    //receives message from the connected server
    char message[24];
    receive_from_server(message);
    send_to_server(message);

//===========================Gate Code End===========================================

//========================Initialise Hardware========================================
    int i;
    init(0);
    // connect camera to the screen
    open_screen_stream();
    // set all didgital outputs to +5V
    for (i = 0; i < 8; i++)
    {
      // set all digital channels as outputs
      select_IO(i,0);
      write_digital(i,1);
    }
//========================Initialise Hardware End====================================

//========================Line Following=============================================
    while(!wall) // keep looping
   {
	int test = 1000-read_analog(0);
	//printf("test %d\n",test);
	if(!(test>700)){
		wall = true;
	}
       int nwp = 0; // keeps count of the number of white pixels in the picture taken
       take_picture();      // take camera shot
       int white[320]; // array for knowing which pixel is white or black.
       int value;
       // we made the array 320 because that is the width of pixels
       // draw some line
       for(int i = 0; i < 320; i++){
                multiplier[i] = i - 160;
       }
       int leftNwp = 0;
	int rightNwp = 0;
       for(int i = 0; i < 240; i++){
                int valueVertical = get_pixel(300,i,3); // give each pixel in the array the pixel value of its $
                if(valueVertical > threshold){ // change 70 to actual white line value later
                        leftNwp++;
                }
       }

	//for(int i = 0; i < 240; i++){
          //      int valueVertical = get_pixel(320,i,3); // give each pixel in the array the pixel value of its $
            //    if(valueVertical > threshold){ // change 70 to actual white line value later
                   //     rightNwp++;
              //  }
      // }

	if(leftNwp >0){ //&&rightnwpp>0
		lineLeftSideVertical = true;
        }
	else{
		lineLeftSideVertical = false;
	}
       for(int i = 0; i < 320; i++){
            //set_pixel(i, 55 ,255,0,0);//redline
            value = get_pixel(i,130,3); // give each pixel in the array the pixel value of its $
            if(value > threshold){ // change 70 to actual white line value later
                white[i] = 1;
                nwp++;
            }
            else{
                white[i] = 0;
            }
            //printf("%d\n",white[i]); // print array results
        }
//===========red check============
/*
        for(int i = 0; i < 320; i++){//rgb code
            //set_pixel(i, 55 ,255,0,0);//redline
            r = get_pixel(i,130,0); 
	    g = get_pixel(i,130,1); 
            b = get_pixel(i,130,2); 
	        if((r > 270)&&(g<270&&b<270) ){ // change 70 to actual white line value later
                	red = true;
            }
        }
*/
 //============red check end============
 
        //process the data collected so far:
        //
        int proportionalSignal = 0;
        int sumOfError = 0;
        for(int i=0;i<320;i++){
            sumOfError  = sumOfError + multiplier[i]*white[i];
        }
     if(nwp!=0){ // if it detects the line, the do the normal PID code
       sumOfError =  sumOfError/nwp;
       proportionalSignal = sumOfError*kp;
       //printf("RM %d\n", RM);  
       LM = baseSpeed + (proportionalSignal);
       RM = baseSpeed + (-1*(proportionalSignal));
       //printf("RM %d\n", RM);  
	//printf("LM %f\n",LM);
       set_motor(1, RM); 
       set_motor(2, LM); 
     }
     else if(nwp==0){ // number of pixels being zero means it lose the line, so call the method for dealing with that situation
     		if(lineLeftSideVertical){
			turnLeft();
     		}
     		else{
     			turnRight();
     		}
     	//deadEnd(); 
     	}
        //display picture
       update_screen();
	
       Sleep(0,1000);
       for (i = 0 ; i < 8; i++)
       {
       int av = read_analog(i);
      // printf("ai=%d av=%d\n",i,av);
       }
     }
//======================Line Following End==============================================================
   // terminate hardware
//=======================Maze Code=======================================

while(true){
	irProportionalSignal = 0;
	left = 1000 - read_analog(0);
        right = 1000 - read_analog(1);
        irError = left-right;
        irProportionalSignal = irError*0.06;
        printf("left sensor %d\n",left);
        printf("right sensor %d\n",right);
	LM = baseSpeed + (irProportionalSignal);
        RM = baseSpeed + (-1*(irProportionalSignal));
       //printf("RM %d\n", RM);  
        //printf("LM %f\n",LM);
        set_motor(1, RM); 
        set_motor(2, LM); 

//================maze code end=======================================
}
    close_screen_stream();
    set_motor(1,0);
    set_motor(2,0);
  
    return 0;


}
//not used void crossRoad(){
	//set_motor(1,-35);
	//set_motor(2,-35);
	//Sleep(0,500000);
	//set_motor(1,45);
	//set_motor(2,-45);
	//Sleep(1,000000);
//}
/*not used void deadEnd(){
	set_motor(1,reverseSpeed);
	set_motor(2,reverseSpeed);
	Sleep(0,500000);
        //set_motor(1,0);
        //set_motor(2,0);
        //Sleep(0,500000);
        //checkLine();
        if (checkLine()){ // it calls the checkLine() method and if it returns true, turn left. If it isnt, then proceed to normal PID
		set_motor(1,45); 
       		set_motor(2,-45*1.2);
                Sleep(1,00000);
	}
	if (stuck == 7){
                rotate = true;
                while(rotate){
                        int valueStuck = 0;
                        take_picture(); // take camera shot
                        set_motor(1,-turnSpeedStuck);
                        set_motor(2,turnSpeedStuck);
                        //set_pixel(i, 55 ,255,0,0);//redline
                        for(int i = 0; i < 240; i++){
                                valueStuck = get_pixel(160,i,3); // give each pixel in the array the pixel value of its $
                                if(valueStuck > threshold){ // change 70 to actual white line value later       changed number 110 to threshold variable
                                        rotate = false;
                                        stuck = 0;
                                }
                        }
                        update_screen();
                        Sleep(0,40000);
                        //printf("%d\n",white[i]); // print array results
                }
       }
}*/
/*not used bool checkLine(){
	int valueForCheck = 0; // intialise value
        int nwpForCheck = 0; // intialise number of pixels of check after reversing.
        take_picture();
       for(int i = 0; i < 320; i++){ //320 because its the camera pixel width
            set_pixel(i, 55 ,255,0,0); //redline
            valueForCheck = get_pixel(i,56,3); //set values of pixel
            if(valueForCheck > threshold){  //checks for how many white pixels there are in its current position (after reversing)      Changed the value 100 to the 'Threshold' variable
                 nwpForCheck++;
            }
        }
	if(nwpForCheck > 200){
		return true;
	}
	else{
		return false;
	}
}*/

	//double currentError = sumOfError;
        //unsigned long currentTime = getTime();
        //if (lastTime){
        //derivativeSignal = kd*((lastError - currentError) / ( lastTime - currentTime));
        //double test = ( lastTime - currentTime);
        //printf("time %d\n", test);
        //printf("Derivative L  %d\n", derivativeSignal);  
        //}
        //else{
        //derivativeSignal = 0;
        //printf("Derivative E  %d\n", derivativeSignal);  
        //}
        //lastError = currentError;
        //lastTime = currentTime;

/* not usedunsigned long getTime(){
        struct timeval now;
        gettimeofday(&now, NULL);       
        return ((unsigned long) now.tv_sec * 1000 * 1000 + (unsigned long) now.tv_usec);  // combine sec and usec
}*/

void turnLeft(){
	set_motor(1,baseSpeed);
	set_motor(2,baseSpeed);
	//Sleep(0,95000);
	rotate = true;
        while(rotate){
                int valueStuck = 0;
                take_picture(); // take camera shot
                set_motor(1,-turnSpeedStuck);
                set_motor(2,turnSpeedStuck*1.2);
                //set_pixel(i, 55 ,255,0,0);//redline
	//	for(int i = 0; i < 240; i++){
                        valueStuck = get_pixel(160,5,3); //keeps turning left until finds line
                        if(valueStuck > threshold){
                                rotate = false;
                        }
		//	else{
		//		turnRight();
		//	}	rotate = false;
          //      }
                update_screen();
                //Sleep(0,40000);
		//lineLeftSideVertical = false;
                //printf("%d\n",white[i]); // print array results
        }
}
void turnRight(){
	set_motor(1,baseSpeed);
        set_motor(2,baseSpeed);
        //Sleep(1,000000);
	rotate = true;
        while(rotate){
                int valueStuck = 0;
                take_picture(); // take camera shot
                set_motor(1,turnSpeedStuck);
                set_motor(2,-turnSpeedStuck*1.2);
                //for(int i = 0; i < 240; i++){
//		Sleep(0,40000);
                        valueStuck = get_pixel(160,5,3); // keeps turning right until line found
                        if(valueStuck > threshold){ // change 70 to actual white line value later
                                rotate = false;
                        }
                //}
                update_screen();
                //Sleep(1,000000);
		//lineLeftSideVertical = true;
                //printf("%d\n",white[i]); // print array results
        }
}

