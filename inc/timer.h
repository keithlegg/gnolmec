#ifndef TIMER_H    
#define TIMER_H





#include <sys/time.h>

void timer_test(void);




// time_t         tv_sec      seconds
// suseconds_t    tv_usec     microseconds
// The <sys/time.h> header defines the itimerval structure that includes at least the following members:
// struct timeval it_interval timer interval
// struct timeval it_value    current value


/*
   FROM HERE 

   http://www.songho.ca/misc/timer/timer.html
 
*/


class timer
{
    public:
        
        timer(){
            startCount.tv_sec = startCount.tv_usec = 0;
            endCount.tv_sec = endCount.tv_usec = 0;

            stopped = 0;
            startTimeInMicroSec = 0;
            endTimeInMicroSec = 0;
        }

        ~timer(){};


        void   start();                             // start timer
        void   stop();                              // stop the timer
        double getElapsedTime();                    // get elapsed time in second
        double getElapsedTimeInSec();               // get elapsed time in second (same as getElapsedTime)
        double getElapsedTimeInMilliSec();          // get elapsed time in milli-second
        double getElapsedTimeInMicroSec();          // get elapsed time in micro-second

    protected:

    private:
        double startTimeInMicroSec;                 // starting time in micro-second
        double endTimeInMicroSec;                   // ending time in micro-second
        int    stopped;                             // stop flag 

        timeval startCount;                         
        timeval endCount;                            



};






#endif
