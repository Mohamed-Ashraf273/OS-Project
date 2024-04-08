#include "headers.h"
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
void clearResources(int);
// make them global so than clearResources can use threm to kill scheduler and clock
int clk_id;
int scheduler_id;
int msg_id;
struct Process // to store process information and send them to scheduler
{
  int arrive_time;  // process arival time
  int running_time; // needed time to run this process
  int priority;     // priority of the process
  int process_id;   // process id
  // int process_memory;
};

int main(int argc, char *argv[])
{

  signal(SIGINT, clearResources);
  int algrithm_number;
  int slice = 0;
  char algorithm[20], slic_num[20];
  // TODO Initialization
  // 1. Read the input files.
  /**Read file
   * 
   * 
   * ALi makes this and store information od each process in struct and 
   * then put it in queue or dara structure to send it to scdeduler
   */

  // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.  //done
  printf("Please Enter which algorithm do you want   \n");
  printf("1 is for RR \n");
  printf("2 is for SRTN \n");
  printf("3 is for HPF \n");
  scanf("%d", &algrithm_number);
  if (algrithm_number == 1)
  {
    printf("RR needs a time slice it is numbers of seconds that RR runs a process please enter it \n");
    scanf("%d", &slice);
    printf("%d\n", slice);
  }

  sprintf(algorithm, "%d", algrithm_number); // to send this information to scheduker
  sprintf(slic_num, "%d", slice);
  // 3. Initiate and the scheduler and clock processes.
  // 3. Initiate clock process //done.

  msg_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT); // This is the IPC communication between scheduler and process generator
  clk_id = fork();
  if (clk_id == -1)
  {
    printf("Error when try to create clock \n");
    exit(-1);
  }
  if (clk_id == 0)
  {
    execl("clk", "clk.out", NULL);
  }
  else
  {
    // 3. Initiate SCheduler process //done.

    scheduler_id = fork(); // create scheduler process
    if (scheduler_id == -1)
    {
      printf("Error when try to create scheduler \n");
      exit(-1);
    }
    else if (scheduler_id == 0)
    {

      execl("scheduler", "scheduler.out", algorithm, slic_num, NULL);
    }
    else
    {
      initClk();
      int prev = 0; // initial time is equal to 0
      int x;
      // To get time use this
      while (1)
      {
        // 4. Use this function after creating the clock process to initialize clock

        x = getClk(); // if he read the same time
        if (x != prev)
        {
          prev = x;
          printf("current time is %d\n", x);
          // TODO Generation Main Loop
          // 5. Create a data structure for processes and provide it with its parameters.
          // 6. Send the information to the scheduler at the appropriate time.
        }
      }
      destroyClk(true);
    }
  }
}

void clearResources(int signum) // 7. Clear clock resources //done
{
  msgctl(msg_id, IPC_RMID, (struct msqid_ds *)0);

  kill(clk_id, SIGINT);
  kill(scheduler_id, SIGINT);

  // TODO Clears all resources in case of interruption
  exit(0);
}
