#include <time.h>
// these load specific methods from the ENGR101 library
extern "C" int InitHardware ();
extern "C" int ReadAnalog ( int ch_adc ); //infrared
extern "C" int Sleep ( int sec , int usec );
extern "C" int SetMotor ( int motor , int dir , int speed ); //motor
int main (){
// This sets up the RPi hardware and ensures
// everything is working correctly
InitHardware ();

int adc_reading = ReadAnalog(0); //initiate infrared

    printf("%d\n" ,adc_reading);

//sets up continuous while loop for reading from analog (infrared)
    while(1){
    adc_reading = ReadAnalog(0); 
    
//starts motor if infrared sensor reads value > 40
    if(adc_reading > 40){ 
            SetMotor (1 , 1, 100);
            Sleep (1,500000);
    }
//stops motor if infrared sensor reads no value    
    else{
            SetMotor (1 , 1, 0);
    }
    }


return 0;}
