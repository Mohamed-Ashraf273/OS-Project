#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int *shr;
void ClearCock(int signum) // if it receive interrupt signal
{
    shmdt(shr);
    destroyClk(false);

    exit(0);
}
void StopHandler(int signum)
{
    *shr = remainingtime;
    raise(SIGSTOP);
}
int main(int agrc, char *argv[])
{
    signal(SIGINT, ClearCock);
    signal(SIGTSTP, StopHandler);
    initClk();
    int shm_id = atoi(argv[1]);
    remainingtime = atoi(argv[2]); // first time
    // TODO it needs to get the remaining time from somewhere(sheduler)
    // remainingtime = ??;
    shr = shmat(shm_id, NULL, 0);
    int prev = getClk();
    printf("time=%d\n", prev);
    while (remainingtime > 0)
    {
        int x = getClk();

        if (x != prev)
        {
            prev = x;
            x = getClk();
            remainingtime--; // descrease its value
            printf("at clock= %d remining  =%d running time %d\n", x, remainingtime, atoi(argv[2]));
            // remainingtime = ??;
        }
    }
    /////if it normally finished
    shmdt(shr);
    destroyClk(false);

    exit(0);
}
