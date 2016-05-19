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

void deadEnd();
void crossRoad();


int v_left = 0;
int v_right = 0;
int LM;
int RM;

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
    int baseSpeed = 32;
    float kp = 0.4; //p constant
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
       int value;
       // we made the array 320 because that is the width of pixels
       // draw some line
       for(int i = 0; i < 320; i++){
            set_pixel(i, 55 ,255,0,0);//redline
            value = get_pixel(i,56,3); // give each pixel in the array the pixel value of its $
            if(value > 100){ // change 70 to actual white line value later
                white[i] = 1;
                nwp++;
            }
            else{
                white[i] = 0;
            }
            //printf("%d\n",white[i]); // print array results
        }
         
 
        //process the data collected so far:
        //
        int proportionalSignal = 0;
        int sumOfError = 0;
        for(int i=0;i<320;i++){
            sumOfError  = sumOfError + (i - 160)*white[i];
        }
     if(nwp!=0){
       sumOfError =  sumOfError/nwp;
       proportionalSignal = sumOfError*kp;
       printf("sum of error*kp %d\n", sumOfError*kp);  
       LM = baseSpeed + (proportionalSignal);
       RM = baseSpeed + (-1*(proportionalSignal));
       printf("%d\n",LM);
       set_motor(1, LM); 
       set_motor(2, RM); 
     }
     else if(nwp==0){
     	deadEnd();
     }
     else if(nwp > 200){
     	crossRoad();
     }
     
     //printf("%d\n",proportionalSignal);
     



       // display picture
       update_screen();
       Sleep(0,100000);
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

// below is hardcode

void crossRoad(){
	set_motor(1,-60);
	set_motor(2,-60);
	Sleep(1,00000);
	set_motor(1,-60);
	set_motor(2,60);
	Sleep(1,100000);
}
void deadEnd(){
	set_motor(1,-40);
	set_motor(2,-40);
	Sleep(1,0);
	set_motor(1,40);
	set_motor(2,-20);
	Sleep(0,10000000);
}
