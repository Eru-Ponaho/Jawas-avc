    int i;
    int baseSpeed = 35;
    float kp =0.1; // 'P' constant for PID
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
         
        //process the data collected so far:
        int e=0;
        int sum = 0;
        for(int i=0;i<320;i++){
            sum  = sum + (i - 160)*white[i];
        }
 
     e = (sum/20)*kp;            
     int LM = baseSpeed+e;
     int RM = baseSpeed+(-1*e);
     printf("%d\n",LM);
     set_motor(1, LM); 
     set_motor(2, RM); 
}
