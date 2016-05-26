#include <stdio.h>
#include <sys/time.h>
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

int turnSpeedStuck = 39;
int reverseSpeed = -35;
int turnSpeed = 35;
int LM = 0;
int RM = 0;
void crossRoad();
void deadEnd();
bool checkLine();
unsigned long getTime();
bool rotate = false;
int stuck = 0;
int threshold = 130;
unsigned long lastTime;
double lastError;
//int derivativeSignal;

int main()
{

    //Open gate:
    //connects to server
   // connect_to_server("130.195.6.196", 1024);
   //sends a message to the connected server
    //send_to_server("Please");
    //send_to_server("123456");
    //receives message from the connected server
    //char message[24];
    //receive_from_server(message); //this line looks buggy, is it right?
    //send_to_server(message);
    int i;
    int baseSpeed = 33; // Seemed good at 35
    float kp = 0.2; //p constant - was good at 0.189
    float kd = 0;
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
    while(1)
    {
       int nwp = 0;
       take_picture();      // take camera shot
       int white[320];
       int multiplier[320];
       int value=0;
	int value2=0;
	int value3=0;
       int mean = 0;
       // we made the array 320 because that is the width of pixels
       // draw some line
	for(int i = 0; i < 3; i++){
		for(int i = 0; i < 320; i++){
           		 set_pixel(i,55,255,0,0);//redline
	    		//take_picture(); 
            		value = get_pixel(i,56,3); // give each pixel in the array the pixel value of its $
			mean += value;
		}
	mean = mean/3;
	for(int i = 0; i < 320; i++){
                multiplier[i] = i - 160;
      	   
 		if(mean > threshold){
                	white[i] = 1;
                	nwp++;
            	}
            	else{
                	white[i] = 0;
            	}
            //printf("%d\n",white[i]); // print array results
        	}
        } 
 	
        //process the data collected so far:
        //
	double  derivativeSignal = 0;
        int proportionalSignal = 0;
        int sumOfError = 0;
        for(int i=0;i<320;i++){
            sumOfError  = sumOfError + multiplier[i]*white[i];
        }
       if(nwp!=0){
       sumOfError =  sumOfError/nwp;
//Der
	double currentError = sumOfError;
	unsigned long currentTime = getTime();
	if (lastTime){
	derivativeSignal = kd*((lastError - currentError) / ( lastTime - currentTime));
	double test = ( lastTime - currentTime);
	printf("time %d\n", test);
	printf("Derivative L  %d\n", derivativeSignal);  

	}
	else{
	derivativeSignal = 0;
	printf("Derivative E  %d\n", derivativeSignal);  

	}
	lastError = currentError;
	lastTime = currentTime;
//Pro
       proportionalSignal = sumOfError*kp;
       //printf("D  %d\n", derivativeSignal);  
       LM = baseSpeed + (proportionalSignal-derivativeSignal);
       RM = baseSpeed + (-1*(proportionalSignal-derivativeSignal));
       printf("LM %d\n",LM);
       set_motor(1, RM); 
       set_motor(2, LM); 
//	printf("nwp %d\n",nwp);
    	 }
      else if(nwp==0){
        stuck++;
 	deadEnd();
     }
     //else if(nwp > 200){
     //	crossRoad();
    // }
       // display picture
       update_screen();
       Sleep(0,1000);
       for (i = 0 ; i < 8; i++)
       {
       int av = read_analog(i);
      // printf("ai=%d av=%d\n",i,av);
       }
     }
   // terminate hardware
    close_screen_stream();
    set_motor(1,0);
    set_motor(2,0);
  
    return 0;


}
void crossRoad(){
	//set_motor(1,-35);
	//set_motor(2,-35);
	//Sleep(0,500000);
	//set_motor(1,45);
	//set_motor(2,-45);
	//Sleep(1,000000);
}
void deadEnd(){
	if ( !(checkLine()) && (stuck !=7)){
	set_motor(1,reverseSpeed);
	set_motor(2,reverseSpeed);
	Sleep(0,500000);
	}
        //set_motor(1,0);
        //set_motor(2,0);
        //Sleep(0,500000);
        //checkLine();
        if (checkLine()){
		stuck = 0;
		set_motor(1,-turnSpeed); 
       		set_motor(2,turnSpeed);
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
            			if(valueStuck > 110){ // change 70 to actual white line value later
                			rotate = false;
					stuck = 0;
            			}
			}
			update_screen();
			Sleep(0,40000);
            		//printf("%d\n",white[i]); // print array results
		}
	}
}
bool checkLine(){
	int valueForCheck = 0;
        int nwpForCheck = 0;
        take_picture();
	update_screen();
       // we made the array 320 because that is the width of pixels
       // draw some line
       for(int i = 0; i < 320; i++){
            set_pixel(i, 55 ,255,0,0);//redline
            valueForCheck = get_pixel(i,56,3); // give each pixel in the array the pixel value of its $
            if(valueForCheck > 100){ // change 70 to actual white line value later
                 nwpForCheck++;
            }
            //printf("%d\n",white[i]); // print array results
        }
	if(nwpForCheck > 300){ //changed from 200
		return true;
	}
	else{
		return false;
	}
}


unsigned long getTime(){
	struct timeval now;
	gettimeofday(&now, NULL);	
	return ((unsigned long) now.tv_sec * 1000 * 1000 + (unsigned long) now.tv_usec);  // combine sec and usec
}
