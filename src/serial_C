

#include <fcntl.h>       //?serial related 
#include <termios.h>     //serial port API 

/***************************************/
/*
  cobbled together serial port code to read and write bytes to SEM 
*/

 
int sem_com( char *filename, char *SERIAL_PORT, int mode )
{
    //char *filename    = "/keith/image.bin"; //name of file to save 

    struct termios tio;
    struct termios stdio;
    struct termios old_stdio;
    int tty_fd;

    tcgetattr( STDOUT_FILENO, &old_stdio);

    memset( &stdio, 0, sizeof(stdio) );
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;
    tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // make the reads non-blocking

    memset( &tio,0, sizeof(tio) );
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;    // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    tty_fd=open(SERIAL_PORT, O_RDWR | O_NONBLOCK);      

    cfsetospeed(&tio, B57600);    
    cfsetispeed(&tio, B57600);     
    tcsetattr( tty_fd, TCSANOW, &tio);

    ////////
    unsigned char c='D'; //byte for serial buffer  

    //experimental scan mode 
    if (mode==0)
    {
      int pxl_row = 0;//row iterator
      int pxl_idx = 0;//pixel iterator

      //i think this will calculate the number of pixels ??
      int res = steps[step_idx];
      int numpixels = ((res*2)+1)*(res); 

      unsigned char pxl_buffer[1048576] = {0};

      //ask SEM to start scanning 
      write(tty_fd,"s",1);

      //now listen for 10 bit binary in two bytes [ (8_bits_msb, 2_lsb) ] on serial port 
      int isfinished = 0;
      while(isfinished==0)
      {
        if (read(tty_fd,&c,1)>0)
        {
            //count rows  
            if(c==0xa)
            {
                pxl_row++;
                //printf("%i\n", pxl_row);
            }

            //check number of rows recieved 
            if(pxl_row==steps[step_idx])
            {
                printf("%i rows - success!! \n\r", steps[step_idx]);
                printf("counted # pixels %i\n\r", (pxl_idx+1) ); //add one for zero index
                printf("predicted # pixels %i\n\r", numpixels );
                //printf("total buffer size %i %i \n\r", sizeof(pxl_buffer[0]), sizeof(pxl_buffer)/sizeof(pxl_buffer[0]) );
                                
                //pxl_row = 0; //you will want to reset for next image 
                
                /////

                //dump binary data to disk 
                FILE *file_ptr;    
                file_ptr=fopen(filename, "wb");                
                //fwrite(pxl_buffer, sizeof(pxl_buffer[0]), sizeof(pxl_buffer)/sizeof(pxl_buffer[0]), file_ptr);
                fwrite(pxl_buffer, 1, numpixels, file_ptr);
                int fclose(FILE *file_ptr);
               
                /////

                //dump text data to disk 
                    
                // FILE *file_ptr;    
                // file_ptr=fopen("/keith/dump.txt", "w");                
                // fwrite(pxl_buffer, sizeof(pxl_buffer[0]), sizeof(pxl_buffer)/sizeof(pxl_buffer[0]), file_ptr);
                // int fclose(FILE *file_ptr);
                

                /////
                isfinished=1;
            }

            //write(STDOUT_FILENO,&c,1); //print the byte on the screen 
            pxl_buffer[pxl_idx]=c;
            pxl_idx++;
        }
      }
      //  printf("recieved %i rows of pixels.\n", pxl_row);   
    }//scan mode /0 

    ///////

    //set step size "b" command 
    if (mode==1)
    {
        if(step_idx<7){
            step_idx++;
        }else{
            step_idx=0;
        }
        printf("image size is %d\n", steps[step_idx]);
        write(tty_fd,"b",1);
    }

    //////

    //report internals - "w" command 
    if (mode==2)
    {
        write(tty_fd,"w",1);
    }

    /////

    //test uart echo - "q" command 
    if (mode==3)
    {
        write(tty_fd,"q",1);
        int isfinished = 0;
        if (read(tty_fd,&c,1)>0)
        {
            write(STDOUT_FILENO,&c,1); //print the byte on the screen 
        }
    }

    //close connection and return STDIO 
    close(tty_fd);
    tcsetattr( STDOUT_FILENO, TCSANOW, &old_stdio);
    return EXIT_SUCCESS;

}
 
