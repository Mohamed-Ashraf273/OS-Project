#include "headers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    int remainingTime;
    // int process_memory;
};
int RrunningT;
struct Time
{
    int runnunig_time;
    int start_time;
};
// Node structure
typedef struct Node {
    struct Process  *data;
    struct Node *next;
    struct Node *prev;
} Node;

// Linked list structure
typedef struct {
    Node *head;
    Node *tail;
} LinkedList;
void initializeList(LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
}
void Add(LinkedList *list, struct Process  *data) {//add to head
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    if(list->head==NULL){//empty
        list->head=list->tail=newNode;
        newNode->next=NULL;
        newNode->prev=NULL;
    }else{
        if(list->head->next==NULL){
            list->tail->prev=list->head;
        }
        newNode->next = list->head;
        list->head = newNode;
        
    }
}
void AddSortedPriority(LinkedList *list, struct Process  *data){
     Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    if(list->head==NULL){//empty
        list->head=list->tail=newNode;
        newNode->next=NULL;
    }else{
        if(newNode->data->priority<list->head->data->priority){
            newNode->next = list->head;
            list->head = newNode;
        }
        else{
            Node* current=list->head;
            while(current->next!=NULL && current->next->data->priority<newNode->data->priority){
            current=current->next;
            }
            if (current->next==NULL){
                current->next=newNode;
                list->tail=newNode;
            }
            else{
                newNode->next=current->next;
                current->next=newNode;
            }
        }
    }
}
void SortAccordingRT(LinkedList *list){
    if(list->head==list->tail || list->head==NULL){
        return;
    }else{
        Node *current1 = list->head;
        Node *current2 = list->head->next;
        Node *current3 = list->head;
        while(current3!=list->tail){
            while(current2!=NULL){
                if(current2->data->remainingTime<current1->data->remainingTime){
                    current1->data=current2->data;
                }
                current2=current2->next;
                current1=current1->next;
            }
            current3=current3->next;
        }
    }
}
void RemH(LinkedList* list){//remove from head
    if(list->head!=NULL){
        Node* current=list->head;
        list->head=list->head->next;
        if(list->head==NULL){
            list->tail=list->head;
        }
        current->next=NULL;
        free(current);
    }
}
void RemT(LinkedList* list){//remove from tail
    if(list->tail!=NULL){
        Node* current=list->tail;
        list->tail=list->tail->prev;
        if(list->tail==NULL){
            list->head=list->tail;
        }
        current->next=NULL;
        list->tail->next=NULL;
        free(current);
    }
}
void *shr;
LinkedList Ready;
Node* RunningProcess;
int number_of_finish_process = 0; // number of finished process
void handleChild(int signum)      // this is when a process send this to scheduler
{
    // this occure when process is stoped by scheduler or terminated
    // Time time;
    // memcpy(&time, ( Time *)shr, sizeof(Time));
    printf("start at  %d and run %d\n", ((struct Time *)shr)->start_time,((struct Time *)shr)->runnunig_time);
    RrunningT=((struct Time *)shr)->runnunig_time;
    if(RunningProcess->data->running_time==RrunningT){
        //if process.runningtime = x then 
        //calc. statistics ----> Ali
        //add +1 number_of_finish_process --->mohamed
        number_of_finish_process++;
    }
    RunningProcess->data->remainingTime=RunningProcess->data->running_time-RrunningT;
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
    Node *RRPointer=Ready.head;// pointer points to the first process for RR Algo
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
                    process.child_id = child_id; // set process id

                    if (i == 0) // for first time only
                    {
                        shr = shmat(shm_id, NULL, 0);
                        i++;
                    }
                    process.remainingTime=process.running_time;
                    //Nesma, can we stop the clock here then continuing it below the switch code? 
                    // algorithm of sorting based on scheduler algorithm
                    // here you can stop the running process before sorting in the ready queue
                    // beacuse I think it will take time Mohammed
                    switch (algorithm) {
                        case 1: // Round Robin
                        Add(&Ready,&process);
                        if(i==0){// if itis the first process arrives then run it
                            kill(process.process_id,SIGCONT);
                        }
                        break;
                        case 2: // Shortest Remaining Time Next (SRTN) Sorted
                        if(i!=0){//if not the first time
                            kill(Ready.head->data->process_id,SIGUSR2);
                        }
                        Add(&Ready,&process);
                        SortAccordingRT(&Ready);
                        break;
                        case 3: // Highest Priority First (HPF) Sorted
                        AddSortedPriority(&Ready,&process);
                        if(i==0){// if itis the first process arrives then run it
                            kill(process.process_id,SIGCONT);
                        }
                        //sort list
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
                if(Ready.head->data->running_time == RrunningT){
                    // If the running time of the process at the head matches RrunningT, remove the node pointed by RRPointer
                    Node* current=Ready.head;
                    while(current->next!=RRPointer){
                        current=current->next;
                    }
                    if(RRPointer==Ready.tail){
                        Ready.tail=current;
                        current->next=NULL;
                        free(RRPointer);
                        RRPointer=Ready.head;
                    }else{
                        current->next=RRPointer->next;
                        RRPointer->next=NULL;
                        free(RRPointer);
                        RRPointer=current->next;
                    }
                    }
                    else{
                    if(RrunningT>=slice){
                    //time it runs greater than or equal slice stop it
                    kill(RRPointer->data->process_id,SIGUSR2);
                    RRPointer=RRPointer->next->next;
                    if(RRPointer==NULL){
                        RRPointer=Ready.head;
                    }
                    kill(RRPointer->data->process_id,SIGCONT);
                    }
                }
                    RunningProcess=RRPointer;
                break;
            case 2: // Shortest Remaining Time Next (SRTN)
                kill(Ready.head->data->process_id,SIGCONT);
                if(Ready.head->data->running_time == RrunningT){
                    RemH(&Ready);
                    kill(Ready.head->data->process_id,SIGCONT);
                }
                RunningProcess=Ready.head;
                break;
            case 3: // Highest Priority First (HPF)
                //if processor is idle then run a process
                //else don't run another one just complete the running one
                if(Ready.head->data->running_time == RrunningT){
                    RemH(&Ready);
                    kill(Ready.head->data->process_id,SIGCONT);
                }
                RunningProcess=Ready.head;
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
