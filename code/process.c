#include "headers.h"
#include <string.h>

/* Modify this file as needed*/
int remainingtime;

struct Time
{
    int runnunig_time;
    int start_time;
};
void *shr;
// Time time;
int start = 0;
int running = 0;
void ClearCock(int signum) // if it receive interrupt signal
{
    shmdt(shr);
    destroyClk(false);

    exit(0);
}
void memory()
{

    // memcpy(( int*)shr, &time, sizeof(int));
    ((struct Time *)shr)->start_time = start;
    ((struct Time *)shr)->runnunig_time = running;

    kill(getppid(), SIGUSR1); // to save information and notify scheduler
}
void StopHandler(int signum)
{
    memory();
    raise(SIGSTOP);
}
int main(int agrc, char *argv[])
{
    //printf("helloFromProcess\n");
    signal(SIGINT, ClearCock);
    signal(SIGUSR2, StopHandler);
    int shm_id = atoi(argv[1]);
    remainingtime = atoi(argv[2]); // first time
                                   //  int slice = atoi(argv[3]);
    int actual_runnuing_time = 0;
    // TODO it needs to get the remaining time from somewhere(sheduler)
    // remainingtime = ??;
    shr = shmat(shm_id, NULL, 0);
   kill(getpid(), SIGUSR2); // to stop it initially when create

    if (shr == (void *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    initClk();

    int prev = getClk();
    printf("time=%d\n", prev);
    start = prev;
    // time.start_time = prev; // firts time it runs
    // time.runnunig_time = 0;
    while (remainingtime > 0)
    {
        //printf("remainingTime: %d\n",remainingtime);
        int x = getClk();

        if (x != prev)
        {

            prev = x;
            x = getClk();
            running++;
            // time.runnunig_time++;
            remainingtime--; // descrease its value
            printf("at clock= %d remaining  =%d running time %d\n", x, remainingtime, atoi(argv[2]));
            // remainingtime = ??;
        }
        
    }
    /////if it normally finished
    destroyClk(false);
    memory();
    shmdt(shr);

    exit(0);
}
