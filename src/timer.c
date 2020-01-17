#include <stdlib.h>
#include <stdio.h>


#include "timer.h"




// void timer_test(void)
// {
// 
//     timeval startCount;
//     float foo = gettimeofday(&startCount, NULL);
//     printf("%ld.%06ld\n", startCount.tv_sec, startCount.tv_usec);
// 
// };




void timer::start()
{
    stopped = 0; // reset stop flag

    gettimeofday(&startCount, NULL);

}




void timer::stop()
{
    stopped = 1; // set timer stopped flag


    gettimeofday(&endCount, NULL);

}




double timer::getElapsedTimeInMicroSec()
{

    if(!stopped)
        gettimeofday(&endCount, NULL);

    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;

    return endTimeInMicroSec - startTimeInMicroSec;
}




double timer::getElapsedTimeInMilliSec()
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}




double timer::getElapsedTimeInSec()
{
    return this->getElapsedTimeInMicroSec() * 0.000001;
}




double timer::getElapsedTime()
{
    return this->getElapsedTimeInSec();
}

