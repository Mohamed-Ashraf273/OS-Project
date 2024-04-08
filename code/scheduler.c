#include "headers.h"


int main(int argc, char * argv[])
{
  int algorithm= atoi(argv[1]);
    initClk();
    while(1)
    {
        //
        
        printf("%d\n",algorithm);
        sleep(2);
    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    destroyClk(true);
}
