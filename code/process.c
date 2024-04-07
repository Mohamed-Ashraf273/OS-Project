#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
bool Blocked;
int main(int agrc, char * argv[])
{
    
    initClk();
    
    //TODO it needs to get the remaining time from somewhere(sheduler)
    //remainingtime = ??;
    while (remainingtime > 0)
    {
        if(Blocked){
        //Stop process
    }
        // remainingtime = ??;
    }
    
    destroyClk(false);
    
    return 0;
}
