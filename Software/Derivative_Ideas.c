    int i;
    int baseSpeed = 35;
    float kp =0.1; // 'P' constant for PID
    float kd =  0; // 'D' constant for PID, set to 0 for now because needs testing
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
       take_picture();      // take camera shot
       int white[320];
       int value;
       // we made the array 320 because that is the width of pixels
       // draw some line
       for(int i = 0; i < 320; i++){
            set_pixel(i, 55 ,255,0,0);//redline
            value = get_pixel(i,56,3); // give each pixel in the array the pixel value of its $
            if(value > 100){ // This number (100) is the threshold between black and white
                white[i] = 1;
            }
            else{
                white[i] = 0;
            }
            //printf("%d\n",white[i]); // print array results
        }
         
        //Process the data collected so far:
        
        //
        // Proportional below
        //
        int e=0;
        int sum = 0;
        for(int i=0;i<320;i++){
            sum  = sum + (i - 160)*white[i];
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Derivative below
        //
        int previous_error = 0;    //-This setting to zero might have to be put at the very start of the program, because I 
                                   // think as it is now, the previous error will always be zero when the wile loop goes round.
        int current_error = 0;    
        int derivative_signal; 
        int error = 0;
        for(int i=0;i<320;i++){          // This looks very similar to what we already have above to to the proportional error 
            int error = (i - 160)*white[i]; // Maybe we can combine these in some way?
            current error += error;
        }
        Sleep(0,100000)                 // This could be a problem?
        derivative_signal = (current_error-previous_error/0.1)*kd; //I think this si the equivalent of 'e = (sum/20)*kp;' below
        previous_error = current_error;
        printf("Derivative signal is: %d", derivative_signal );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
        e = (sum/20)*kp;            
        int LM = baseSpeed+e+derivative_signal;    // Have added the derivative_signal to these.
        int RM = baseSpeed+(-1*e)+derivative_signal;
        printf("%d\n",LM);
        set_motor(1, LM); 
        set_motor(2, RM); 
    }
