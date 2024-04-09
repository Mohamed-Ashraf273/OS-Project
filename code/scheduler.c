#include "headers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include"LL.h"
void *shr;
LinkedList Ready;
struct Process        // to store process information and send them to scheduler
{                     // long mtype;//FOR MESSAGE
    int process_id;   // process id
    int arrive_time;  // process arival time
    int running_time; // needed time to run this process
    int priority;     // priority of the process
    int finish;       // finish time
    int start;        // first time it running by cpu
    char state[8];    // state of the process
    int child_id;     // when fork to use it when make signals

    // int process_memory;
};

struct Time
{
    int runnunig_time;
    int start_time;
};



int number_of_finish_process = 0; // number of finished process
void handleChild(int signum)      // this is when a process send this to scheduler
{
    // this occure when process is stoped by scheduler or terminated
    // Time time;
    // memcpy(&time, ( Time *)shr, sizeof(Time));
    printf("start at  %d and runn %d\n", ((struct Time *)shr)->start_time,((struct Time *)shr)->runnunig_time);

    signal(SIGUSR1, handleChild);
}
void ClearCock(int signum) // it process_generator interrupt;
{
    shmdt(shr);

    destroyClk(false); // release lock

    exit(0); // exit
}
int main(int argc, char *argv[])
{
    initializeList(&Ready);
    signal(SIGUSR1, handleChild); // if child terminate process
    signal(SIGINT, ClearCock);
    // char slic_num[10];
    int msg_id = atoi(argv[1]);    // ID OF MESSAGE QUEUE
    int shm_id = atoi(argv[2]);    // ID OF Shared memory
    int algorithm = atoi(argv[3]); // i RR 2 SRTN 3 HPF //Mohammed use this
    int slice = atoi(argv[4]);
    int number_of_system_process = atoi(argv[5]); // all system processes
    char process_run_time[10];                    // to send it to each process
    char share_id[10];                            // to send it to each process
    sprintf(share_id, "%d", shm_id);
    initClk();
    int i = 0;
    int prev = getClk();
    // to check for each second not apart of second
    while (number_of_finish_process < number_of_system_process) // untill finish
    {
        int x = getClk();
        if (prev != x)
        {
            struct Process process;
            int size = sizeof(process.arrive_time) + sizeof(process.priority) + sizeof(process.process_id) + sizeof(process.running_time); // size of recieved message
            int rec = msgrcv(msg_id, &process, size, 0, IPC_NOWAIT);                                                                       // recieve a process
            if (rec != -1)                                                                                                                 // new process I think need to e scheduled                                                                                                          // receive a process
            {
                sprintf(process_run_time, "%d", process.running_time);
                int child_id = fork();
                if (child_id == -1)
                {
                    printf("Error when try to fork \n");
                }
                else if (child_id == 0) // child code
                {
                    execl("process", "process.out", share_id, process_run_time, NULL);
                }
                else // scheduler
                {
                    if (i == 0) // for first time only
                    {
                        shr = shmat(shm_id, NULL, 0);
                        i++;
                    }
                    

                    process.child_id = child_id; // set process id
                    kill(process.child_id,SIGUSR2);
                    //Nesma, can we stop the clock here then continuing it below the switch code? 
                    // algorithm of sorting based on scheduler algorithm
                    // here you can stop the running process before sorting in the ready queue
                    // beacuse I think it will take time Mohammed
                    switch (algorithm) {
                        case 1: // Round Robin

                        break;
                        case 2: // Shortest Remaining Time Next (SRTN)
                        break;
                        case 3: // Highest Priority First (HPF)

                        break;
                        default:
                        printf("Invalid algorithm!\n");
                        break;
                        }
                    
                }
            }

            // here choose which one should be running based on algorithm
            /*if STRN and HPF will take the first process in queue
            //if round robin based on time slice
            //than signal cont this process
            Scheduler algorithm must be here Mohammed
            //when want to stop processes send it user2 signal
            to contoinue it send sigcon signal
            */
            /*
            when want to run a process send it con signal
            */
           switch (algorithm) {
            case 1: // Round Robin
                
                break;
            case 2: // Shortest Remaining Time Next (SRTN)
                
                break;
            case 3: // Highest Priority First (HPF)
                
                break;
            default:
                printf("Invalid algorithm!\n");
                break;
        }



            
            /*ali
             * print output file must be here also
             */
            prev = x;
        }
    }
    // TODO implement the scheduler :)
    // on termination release the clock resources.
    /**
     calculation of second output file must be here
    */

    destroyClk(false);
    shmdt(shr);
    kill(getppid(), SIGINT); // IF scheduler terminate mean all process he fork also terminate so must terminate process_generator
    exit(0);
}
