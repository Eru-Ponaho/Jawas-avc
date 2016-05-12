
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


int v_left = 0;
int v_right = 0;

int main()
{

    //Open gate:
    //connects to server
    //connect_to_server("130.195.6.196", 1024);
    //sends a message to the connected server
    //send_to_server("Please");
    //send_to_server("123456");
    //receives message from the connected server
    //char message[24];
    //receive_from_server(message); //this line looks buggy, is it right?
    //send_to_server(message);
    
    int baseSpeed = 35;
    float kp = 0.03; //random constant
    float kd = 0;
    init(0);
    // connect camera to the screen
    open_screen_stream();
    // set all didgital outputs to +5V
    for (int i = 0; i < 8; i++)
    {
      // set all digital channels as outputs
      select_IO(i,0);
      write_digital(i,1);
    }
    while(1)
    {
       take_picture();      // take camera shot
       int white[320];
       int value;
       // we made the array 320 because that is the width of pixels
       // draw some line
       for(int i = 0; i < 320; i++){
            set_pixel(i, 55 ,255,0,0);//redline
            value = get_pixel(i,56,3); // give each pixel in the array the pixel value of its location based on ' i '.
            if(value > 100){ // change 70 to actual white line value later
                white[i] = 1;
            }
            else{
                white[i] = 0;
            }
            //printf("%d\n",white[i]); // print array results
        }
        
        //process the data collected so far:
        //but first create an array counting from -170 to 170
        int e=0;
	int sum = 0;
        for(int i=0;i<320;i++){
            sum  = sum + (i - 160)*white[i];
        }

	int previous_error;    //-This setting to zero might have to be put at the very start of the program, because I 
                                   // think as it is now, the previous error will always be zero when the wile loop goes round.
        int current_error = 0;    
        int derivative_signal; 
        int error = 0;
        for(int i=0;i<320;i++){          // This looks very similar to what we already have above to to the proportional error 
            error = (i - 160)*white[i]; // Maybe we can combine these in some way?
            current_error += error;
        }
        //Sleep(0,100000);                 // This could be a problem?
        derivative_signal = (current_error-previous_error/0.1)*kd; //I think this si the equivalent of 'e = (sum/20)*kp;' below
        previous_error = current_error;
        printf("Derivative signal is: %d", derivative_signal );

     e = sum*kp;   
     int LM = baseSpeed+e+derivative_signal;
     int RM = baseSpeed+(-1*e)+(-1*derivative_signal);
     printf("%d\n",LM);
     set_motor(1, LM); 
     set_motor(2, RM); 
     
        
            
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
