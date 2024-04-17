#include "headers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include<math.h>
struct Process        // to store process information and send them to scheduler
{                     // long mtype;//FOR MESSAGE
    int process_id;   // process id
    int arrive_time;  // process arival time
    int running_time; // needed time to run this process
    int priority;     // priority of the process
    int finish;       // finish time
    int start;        // first time it running by cpu
    int state;        // state of the process (0:running &  1:stopped or blocked & 2:term)
    int child_id;     // when fork to use it when make signals
    int remainingTime;
    int RRtime; // store the time at when process start or resumed for RR
    bool firsttime;
    // int process_memory;
};
int RrunningT;
int algo;
struct Time
{
    int runnunig_time;
    int start_time;
};
// Node structure
typedef struct Node
{
    struct Process *data;
    struct Node *next;
    struct Node *prev;
} Node;

// Linked list structure
typedef struct
{
    Node *head;
    Node *tail;
} LinkedList;
void initializeList(LinkedList *list)
{
    list->head = NULL;
    list->tail = NULL;
}
void Add(LinkedList *list, struct Process *data)
{ // add to head
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    if (list->head == NULL)
    { // empty
        list->head = list->tail = newNode;
        newNode->next = NULL;
        newNode->prev = NULL;
    }
    else
    {
        if (list->head->next == NULL)
        {
            list->tail->prev = list->head;
        }
        newNode->next = list->head;
        list->head = newNode;
    }
}
void AddT(LinkedList *list, struct Process *data)
{ // add to tail
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (list->head == NULL)
    { // List is empty
        list->head = list->tail = newNode;
    }
    else
    {
        list->tail->next = newNode; // Link the current tail to the new node
        newNode->prev = list->tail; // Update the previous pointer of the new node
        list->tail = newNode;       // Update the tail pointer to the new node
    }
}
// void AddSortedPriority(LinkedList *list, struct Process  *data){
//      Node *newNode = (Node *)malloc(sizeof(Node));
//     if (newNode == NULL) {
//         printf("Memory allocation failed.\n");
//         return;
//     }
//     newNode->data = data;
//     if(list->head==NULL){//empty
//         list->head=list->tail=newNode;
//         newNode->next=NULL;
//     }else{
//        // if(newNode->data->priority<list->head->data->priority){
//            // newNode->next = list->head;
//            // list->head = newNode;
//        // }
//         //else{
//             Node* current=list->head;
//             while(current->next!=NULL && current->next->data->priority<newNode->data->priority){
//             current=current->next;
//             }
//             if (current->next==NULL){
//                 current->next=newNode;
//                 list->tail=newNode;
//             }
//             else{
//                 newNode->next=current->next;
//                 current->next=newNode;
//             }
//         //}
//     }
// }

void AddSortedPriority(LinkedList *list, struct Process *data)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL; // Initialize prev pointer

    if (list->head == NULL)
    { // Empty list
        list->head = list->tail = newNode;
    }
    else
    {
        Node *current = list->head;
        if (list->head->data->arrive_time != data->arrive_time)
        {
            while (current->next != NULL && data->priority > current->next->data->priority)
            {
                current = current->next;
            }
            if (current->next == NULL)
            { // Insert at the end
                current->next = newNode;
                newNode->prev = current;
                list->tail = newNode; // Update tail
            }
            else
            { // Insert in the middle
                newNode->next = current->next;
                current->next->prev = newNode;
                current->next = newNode;
                newNode->prev = current;
            }
        }
        else
        {
            while (current != NULL && data->priority > current->data->priority)
            {

                current = current->next;
            }
            if (current == NULL)
            { // Insert at the end
                current = list->tail;
                current->next = newNode;
                newNode->prev = current;
                list->tail = newNode; // Update tail
            }
            else
            { // Insert in the middle
                if (current == list->head)
                {
                    newNode->next = current;
                    current->prev = newNode;
                    newNode->prev = NULL;
                    list->head = newNode;
                }
                else
                {
                    newNode->next = current;
                    newNode->prev = current->prev;
                    current->prev->next = newNode;
                    newNode->prev = newNode;
                }
            }
        }
    }
}
void SortAccordingRT(LinkedList *list)
{
    if (list->head == NULL || list->head == list->tail)
    {
        return; // Nothing to sort
    }

    Node *current1 = list->head;
    while (current1 != NULL)
    {
        Node *current2 = current1->next;
        Node *minNode = current1; // Assume current node has minimum remaining time

        // Find the node with minimum remaining time starting from current2
        while (current2 != NULL)
        {
            if (current2->data->remainingTime < minNode->data->remainingTime)
            {
                minNode = current2;
            }
            current2 = current2->next;
        }

        // Swap data between current1 and minNode
        struct Process *temp = current1->data;
        current1->data = minNode->data;
        minNode->data = temp;

        current1 = current1->next; // Move to next node
    }
}

void RemH(LinkedList *list)
{ // remove from head
    if (list->head != NULL)
    {
        Node *current = list->head;
        list->head = list->head->next;
        if (list->head == NULL)
        {
            list->tail = list->head;
        }

        current->next = NULL;
        if (list->head != NULL)
            list->head->prev = NULL;

        free(current);
    }
}
void RemT(LinkedList *list)
{ // remove from tail
    if (list->tail != NULL)
    {
        Node *current = list->tail;
        list->tail = list->tail->prev;
        if (list->tail == NULL)
        {
            list->head = list->tail;
        }
        current->next = NULL;
        if (list->tail != NULL)
            list->tail->next = NULL;
        free(current);
    }
}
void printList(LinkedList *list)
{
    if (list->head != NULL)
    {
        Node *current = list->head;
        while (current != NULL)
        {
            printf("%d, ", current->data->remainingTime);
            current = current->next;
        }
    }
}
void *shr;
FILE* scheduler_log;
FILE* scheduler_perf;
LinkedList Ready;
int cont = 1;
Node *RunningProcess = NULL;
int number_of_finish_process = 0; // number of finished process
int processorState = 0;           // 0:idle & 1:working on process
struct statisticsP {
    int RunningTime;
    int TA;
};
struct statisticsP * result; // I will need
//int index = 0;//global refer to the index of next process to be added to result
void ALGO(LinkedList *list)
{
    // printf("hello\n");
    if (list->head != NULL){
    if (RunningProcess == NULL && list->head != NULL)
        processorState = 0;
    if (!processorState)
    { // if idle
      // printf("idel\n");
        RunningProcess = list->head;
        RunningProcess->data->state = 0; // running
        processorState = 1;              // busy
        kill(RunningProcess->data->child_id, SIGCONT);
        //calc for start or resumed process
        if(RunningProcess!=NULL){
            int wait=getClk()-(RunningProcess->data->arrive_time)-((RunningProcess->data->running_time)-(RunningProcess->data->remainingTime));
            if(RunningProcess->data->firsttime){
                fprintf(scheduler_log,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,RunningProcess->data->remainingTime,wait);
                RunningProcess->data->firsttime=false; 
            }else
                fprintf(scheduler_log,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,RunningProcess->data->remainingTime,wait);
        }
    }
    else
    {
        if (RunningProcess != NULL)
        {
            if (RunningProcess->data->state == 2)
            { // terminated
                // printList(&Ready);

                RemH(list);
                if(algo==2){
                    SortAccordingRT(&Ready);
                }
                // printList(&Ready);
                RunningProcess = list->head;
                if (RunningProcess != NULL)
                {
                    RunningProcess->data->state = 1; // blocked
                    processorState = 0;              // idle
                    kill(RunningProcess->data->child_id, SIGCONT);
                    RunningProcess->data->state = 0; // running
                    processorState = 1;              // busy
                    //calc for start or resumed process
                    if(RunningProcess!=NULL){
                        int wait=getClk()-(RunningProcess->data->arrive_time)-((RunningProcess->data->running_time)-(RunningProcess->data->remainingTime));
                        if(RunningProcess->data->firsttime){
                            fprintf(scheduler_log,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,RunningProcess->data->remainingTime,wait);
                            RunningProcess->data->firsttime=false; 
                        }else
                            fprintf(scheduler_log,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,RunningProcess->data->remainingTime,wait);
                    }
                }
            }
        }
    }
    }
}
void handleChild(int signum) // this is when a process send this to scheduler
{
    // RunningProcess->data->state=1;//stopped;
    //  this occure when process is stoped by scheduler or terminated
    //  Time time;
    //  memcpy(&time, ( Time *)shr, sizeof(Time));
    // printf("stop\n");
    RrunningT = ((struct Time *)shr)->runnunig_time;
    if (RrunningT == 0)
    {
        cont = 0;
    }

    if (RunningProcess != NULL)
    {
        if(algo==2){
            SortAccordingRT(&Ready);
        }
        if (RunningProcess->data->running_time == RrunningT)
        {

            printf("Finished\n");
            // if process.runningtime = x then
            // calc. statistics ----> Ali
            int TA = getClk()-(RunningProcess->data->arrive_time);
            int wait = TA - (RunningProcess->data->running_time);
            float WTA = (TA*1.0) / (RunningProcess->data->running_time);
            WTA=roundf(WTA*100)/100; 
            fprintf(scheduler_log,"At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f \n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,wait,TA,WTA);
            // add +1 number_of_finish_process --->mohamed
            number_of_finish_process++;
            // printf("RrunningT: %d",RrunningT);
            RunningProcess->data->state = 2; // term;
            RunningProcess->data->remainingTime = 0;
            //if(algo==2){
                //SortAccordingRT(&Ready);
            //}
            //ALGO(&Ready);
            // printf("nu m%d\n",number_of_finish_process);
        }
        else
            RunningProcess->data->remainingTime = RunningProcess->data->running_time - RrunningT;
        //  ALGO(&Ready);
    }

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
    //printf("hello from scheduler\n");
    Node *RRPointer = Ready.head; // pointer points to the first process for RR Algo
    signal(SIGUSR1, handleChild); // if child terminate process
    signal(SIGINT, ClearCock);
    // char slic_num[10];
    int msg_id = atoi(argv[1]);    // ID OF MESSAGE QUEUE
    int shm_id = atoi(argv[2]);    // ID OF Shared memory
    int algorithm = atoi(argv[3]); // 1 RR 2 SRTN 3 HPF //Mohammed use this
    algo=algorithm;
    int slice = atoi(argv[4]);
    int number_of_system_process = atoi(argv[5]); // all system processes
    char process_run_time[10];                    // to send it to each process
    char share_id[10];                            // to send it to each process
    sprintf(share_id, "%d", shm_id);
    initClk();
    int i = 0;
    int prev = -1;
    scheduler_log=fopen("scheduler.log","w");
    fprintf(scheduler_log,"#At time x process y state arr w total z remain y wait k\n");
    scheduler_perf=fopen("scheduler.perf","w");
    // to check for each second not apart of second
    int num = 0;
    int busytime;
    struct Process *process = (struct Process *)malloc(number_of_system_process * sizeof(struct Process));
    while (number_of_finish_process < number_of_system_process) // untill finish
    {
        

        int x = getClk();
        //if (prev != x)
        //{
            // printf("   \n");// i don't know why but the code won't work without it
            // struct Process process;
            int size = 16;
            //  if(num!=number_of_system_process){
            //    size = sizeof(process[num].arrive_time) + sizeof(process[num].priority) + sizeof(process[num].process_id) + sizeof(process[num].running_time); // size of recieved message
            //}

            // if(RunningProcess!=NULL){printf("RunningP1: %d\n",RunningProcess->data->priority);}

            while (num != number_of_system_process && msgrcv(msg_id, &process[num], size, 0, IPC_NOWAIT) != -1) // recieve a process                                                                                                               // new process I think need to e scheduled                                                                                                          // receive a process
            {
                //printf("clock1: %d\n",getClk());
                // printf("prio: %d & clock: %d\n",process[num].priority,getClk());
                // if(RunningProcess!=NULL){printf("RunningP2: %d\n",RunningProcess->data->priority);}
               ///
               
                sprintf(process_run_time, "%d", process[num].running_time);
                busytime+=process[num].running_time;
                int child_id = fork();
                // stop
                if (child_id == -1)
                {
                    printf("Error when try to fork \n");
                }
                else if (child_id == 0) // child code
                {
                    //printf("childEntered\n");
                    execl("process", "process", share_id, process_run_time, NULL);
                }
                else // scheduler
                {
                    process[num].child_id = child_id; // set process id
                    //printf("schedulerEntered\n");
                    // printf("process prio: %d\n",process[num].priority);
                    if (i == 0) // for first time only
                    {
                        shr = shmat(shm_id, NULL, 0);
                        i++;
                    }
                    process[num].remainingTime = process[num].running_time;
                    process[num].state = 1;
                    // Nesma, can we stop the clock here then continuing it below the switch code?
                    //  algorithm of sorting based on scheduler algorithm
                    //  here you can stop the running process before sorting in the ready queue
                    //  beacuse I think it will take time Mohammed
                    switch (algorithm)
                    {
                    case 1: // Round Robin
                        AddT(&Ready, &process[num]);
                        if (i == 0 || Ready.head->next == NULL)
                        { // at first the RunningProcess points for the firstProcess
                            RunningProcess = Ready.head;
                        }
                        num++;
                        break;
                    case 2: // Shortest Remaining Time Next (SRTN) Sorted
                        if (RunningProcess != NULL)
                        {
                            if (RunningProcess->data->remainingTime != 0)
                            {
                                kill(RunningProcess->data->child_id, SIGUSR2);
                            }
                            else
                            {
                                RemH(&Ready);
                            }
                            processorState = 0;
                        }
                        //printf("clock2: %d\n",getClk());
                        Add(&Ready, &process[num]);
                        num++;
                        break;
                    case 3: // Highest Priority First (HPF) Sorted
                        // printf("hello from HPF\n");
                        AddSortedPriority(&Ready, &process[num]);
                        num++;
                        //printList(&Ready);
                        // if(!processorState){//if its idle enter
                        // processorState=1;//busy
                        // RunningProcess=Ready.head;
                        // printf("startProcess\n");
                        // kill(RunningProcess->data->process_id,SIGCONT);
                        // process.state=0;
                        //}
                        // sort list
                        break;
                    default:
                        printf("Invalid algorithm!\n");
                        break;
                    }
                    // wait(NULL);
                    //printf("clock1: %d\n",getClk());
                    while (cont){}
                    //printf("clock1: %d\n",getClk());
                    cont = 1;
                }
            }
            //printf("clock2: %d\n",getClk());

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
           
            switch (algorithm)
            {
            case 1: // Round Robin
                if (Ready.head != NULL)
                {
                    if (!processorState)
                    { // Idle, select the first process in the ready queue
                        RunningProcess = Ready.head;
                        processorState = 1; // Busy
                        kill(RunningProcess->data->child_id, SIGCONT);
                        RunningProcess->data->state = 0; // Running
                        RunningProcess->data->RRtime=getClk();
                    }
                    else
                    {
                        if(RunningProcess!=NULL)
                        // Check if the current process has consumed its time slice
                        if (RunningProcess->data->RRtime - getClk() == slice)
                        {
                            kill(RunningProcess->data->child_id, SIGUSR2); // Stop the current process
                            //calc for stopped process
                            int wait=getClk()-(RunningProcess->data->arrive_time)-((RunningProcess->data->running_time)-(RunningProcess->data->remainingTime)); 
                            fprintf(scheduler_log,"At time %d process %d stoped arr %d total %d remain %d wait %d\n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,RunningProcess->data->remainingTime,wait); 
                            //
                            RunningProcess = RunningProcess->next;         // Move to the next process

                            // If reached the end of the ready queue, wrap around to the head
                            if (RunningProcess == NULL && Ready.head != NULL)
                            {
                                RunningProcess = Ready.head;
                            }

                            // If there's a process to run, continue its execution
                            if (RunningProcess != NULL && RunningProcess->data->remainingTime != 0)
                            {
                                RunningProcess->data->state = 1; // Blocked
                                processorState = 0;              // Idle
                                kill(RunningProcess->data->child_id, SIGCONT);
                                RunningProcess->data->state = 0; // Running
                                processorState = 1;              // Busy
                                //calc for start or resumed process
                                if(RunningProcess!=NULL){
                                    int wait=getClk()-(RunningProcess->data->arrive_time)-((RunningProcess->data->running_time)-(RunningProcess->data->remainingTime));
                                    if(RunningProcess->data->firsttime){
                                        fprintf(scheduler_log,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,RunningProcess->data->remainingTime,wait);
                                        RunningProcess->data->firsttime=false; 
                                    }else
                                        fprintf(scheduler_log,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),RunningProcess->data->process_id,RunningProcess->data->arrive_time,RunningProcess->data->running_time,RunningProcess->data->remainingTime,wait);
                                }
                            }
                            else if(RunningProcess != NULL)
                            {
                                RunningProcess->data->state = 1; // Blocked
                                processorState = 0;              // Idle
                            }
                        }
                    }
                }
                else
                {
                    processorState = 0; // Idle
                }
                break;

            case 2: // Shortest Remaining Time Next (SRTN)
                // kill(Ready.head->data->process_id,SIGCONT);
                // if(Ready.head->data->running_time == RrunningT){
                //  RemH(&Ready);
                // kill(Ready.head->data->process_id,SIGCONT);
                //}
                // RunningProcess=Ready.head;
              //  printList(&Ready)
              
                if (Ready.head != NULL)
                {
                    // printf("State: %d\n",RunningProcess->data->state);
                    //printf("clock3: %d\n",getClk());
        //             printf("number_of_system_process: %d\n",number_of_system_process);
        //    printf("number_of_finish_process: %d\n",number_of_finish_process);
                    SortAccordingRT(&Ready);
                    ALGO(&Ready);
                }
                else
                {
                    processorState = 0; // idle
                }
                break;
            case 3: // Highest Priority First (HPF)
                    //   printf("hello from case3\n");
                if (Ready.head != NULL)
                {
                    // printf("State: %d\n",RunningProcess->data->state);
                    ALGO(&Ready);
                }
                else
                {
                    processorState = 0; // idle
                  //  ALGO(&Ready);
                }
                // printf("State: %d\n",RunningProcess->data->state);
                // if(RunningProcess->data->state==2){
                // printf("hi\n");
                // RemH(&Ready);
                // RunningProcess=Ready.head;
                // if(RunningProcess!=NULL){
                // kill(RunningProcess->data->process_id,SIGCONT);
                //}
                //}
                break;
            default:
                printf("Invalid algorithm!\n");
                break;
            }

            prev = x;

            /*ali
             * print output file must be here also
             */ 
            //I think this place is not suitable because I won't know if process resumed or start or just continue running from last time 
        //}
    }
    // TODO implement the scheduler :)
    // on termination release the clock resources.
    /**
     calculation of second output file must be here
    */
    float CPUuti = 1.0*busytime/getClk();
    CPUuti=roundf(CPUuti*10000)/100;
    fprintf(scheduler_perf,"CPU utilization = %.2f\n",CPUuti);
    
    //  printf("scheduler\n");
    shmdt(shr);
    fclose(scheduler_log);
    fclose(scheduler_perf);
    destroyClk(false);
    kill(getppid(), SIGINT); // IF scheduler terminate mean all process he fork also terminate so must terminate process_generator
    exit(0);
}
