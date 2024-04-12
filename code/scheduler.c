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

void AddSortedPriority(LinkedList* list, struct Process* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL; // Initialize prev pointer

    if (list->head == NULL) { // Empty list
        list->head = list->tail = newNode;
    }
    else {
        Node* current = list->head;
        while (current->next != NULL && data->priority > current->next->data->priority) {
            current = current->next;
        }
        if (current->next == NULL) { // Insert at the end
            current->next = newNode;
            newNode->prev = current;
            list->tail = newNode; // Update tail
        }
        else { // Insert in the middle
            newNode->next = current->next;
            current->next->prev = newNode;
            current->next = newNode;
            newNode->prev = current;
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
        list->head->prev=NULL;
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
void printList(LinkedList* list){
    if(list->head!=NULL){
        Node* current=list->head;
        while(current!=NULL){
            printf("%d, ",current->data->priority);
            current=current->next;
        }
    }
}
void *shr;
LinkedList Ready;
int cont=1;

Node* RunningProcess=NULL;
int number_of_finish_process = 0; // number of finished process
void handleChild(int signum)      // this is when a process send this to scheduler
{
    //RunningProcess->data->state=1;//stopped;
    // this occure when process is stoped by scheduler or terminated
    // Time time;
    // memcpy(&time, ( Time *)shr, sizeof(Time));
    //printf("start at  %d and run %d\n", ((struct Time *)shr)->start_time,((struct Time *)shr)->runnunig_time);
    RrunningT=((struct Time *)shr)->runnunig_time;
        if(RrunningT==0){
            cont=0;
        }
        
    if(RunningProcess!=NULL){
    if(RunningProcess->data->running_time==RrunningT){
        printf("Finished\n");
        //if process.runningtime = x then 
        //calc. statistics ----> Ali
        //add +1 number_of_finish_process --->mohamed
        number_of_finish_process++;
        //printf("RrunningT: %d",RrunningT);
        RunningProcess->data->state=2;//term;
    }
    RunningProcess->data->remainingTime=RunningProcess->data->running_time-RrunningT;
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
    int processorState=0;//0:idle & 1:working on process
    initializeList(&Ready);
    //printf("hello from scheduler\n");
    Node *RRPointer=Ready.head;// pointer points to the first process for RR Algo
    signal(SIGUSR1, handleChild); // if child terminate process
    signal(SIGINT, ClearCock);
    // char slic_num[10];
    int msg_id = atoi(argv[1]);    // ID OF MESSAGE QUEUE
    int shm_id = atoi(argv[2]);    // ID OF Shared memory
    int algorithm = atoi(argv[3]); // 1 RR 2 SRTN 3 HPF //Mohammed use this
    int slice = atoi(argv[4]);
    int number_of_system_process = atoi(argv[5]); // all system processes
    char process_run_time[10];                    // to send it to each process
    char share_id[10];                            // to send it to each process
    sprintf(share_id, "%d", shm_id);
    initClk();
    int i = 0;
    int prev = 0;
    // to check for each second not apart of second
    int num=0;
    struct Process* process=(struct Process*)malloc(number_of_system_process*sizeof(struct Process));
    while (number_of_finish_process < number_of_system_process) // untill finish
    {
        int x = getClk();
        if (prev != x)
        {
            printf("   \n");// i don't know why but the code won't work without it
            // struct Process process;

            int size = sizeof(process[num].arrive_time) + sizeof(process[num].priority) + sizeof(process[num].process_id) + sizeof(process[num].running_time); // size of recieved message
           
           //if(Ready.head!=NULL){printf("ReadHead1: %d\n",Ready.head->data->priority);}

           while( msgrcv(msg_id, &process[num], size, 0, IPC_NOWAIT)!=-1 && num!=number_of_system_process)                                                                    // recieve a process                                                                                                               // new process I think need to e scheduled                                                                                                          // receive a process
            {
                //printf("prio: %d\n",process[num].priority);
                //if(Ready.head!=NULL){printf("ReadHead2: %d\n",Ready.head->data->priority);}
                sprintf(process_run_time, "%d", process[num].running_time);
                int child_id = fork();
                //stop
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
                    //printf("process prio: %d\n",process[num].priority);
                    if (i == 0) // for first time only
                    {
                        shr = shmat(shm_id, NULL, 0);
                        i++;
                    }
                    process[num].remainingTime=process[num].running_time;
                    process[num].state=1;
                    //Nesma, can we stop the clock here then continuing it below the switch code? 
                    // algorithm of sorting based on scheduler algorithm
                    // here you can stop the running process before sorting in the ready queue
                    // beacuse I think it will take time Mohammed
                    switch (algorithm) {
                        case 1: // Round Robin
                        
                        break;
                        case 2: // Shortest Remaining Time Next (SRTN) Sorted
                        
                        break;
                        case 3: // Highest Priority First (HPF) Sorted
                        //printf("hello from HPF\n");
                        

                        AddSortedPriority(&Ready,&process[num]);
                        num++;
                        printList(&Ready);
                        //if(!processorState){//if its idle enter
                            //processorState=1;//busy
                            //RunningProcess=Ready.head;
                            //printf("startProcess\n");
                            //kill(RunningProcess->data->process_id,SIGCONT);
                            //process.state=0;
                        //}
                        //sort list
                        break;
                        default:
                        printf("Invalid algorithm!\n");
                        break;
                        }
                        //wait(NULL);
                        while(cont){}
                        cont=1;
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
                //if(Ready.head->data->running_time == RrunningT){
                    // If the running time of the process at the head matches RrunningT, remove the node pointed by RRPointer
                    //Node* current=Ready.head;
                    //while(current->next!=RRPointer){
                        //current=current->next;
                    //}
                    //if(RRPointer==Ready.tail){
                        //Ready.tail=current;
                        //current->next=NULL;
                        //free(RRPointer);
                        //RRPointer=Ready.head;
                    //}else{
                        //current->next=RRPointer->next;
                        //RRPointer->next=NULL;
                        //free(RRPointer);
                       // RRPointer=current->next;
                    //}
                    //}
                    //else{
                    //if(RrunningT>=slice){
                    //time it runs greater than or equal slice stop it
                    //kill(RRPointer->data->process_id,SIGUSR2);
                    //RRPointer=RRPointer->next->next;
                    //if(RRPointer==NULL){
                        //RRPointer=Ready.head;
                    //}
                    //kill(RRPointer->data->process_id,SIGCONT);
                    //}
                //}
                    //RunningProcess=RRPointer;
                break;
            case 2: // Shortest Remaining Time Next (SRTN)
                //kill(Ready.head->data->process_id,SIGCONT);
                //if(Ready.head->data->running_time == RrunningT){
                   // RemH(&Ready);
                    //kill(Ready.head->data->process_id,SIGCONT);
                //}
                //RunningProcess=Ready.head;
                break;
            case 3: // Highest Priority First (HPF)
                //printf("hello from case3\n");
                if(Ready.head!=NULL){
                    //printf("State: %d\n",RunningProcess->data->state);
                    if(!processorState){// if idle
                       RunningProcess=Ready.head;
                       RunningProcess->data->state=0;//running
                       processorState=1;//busy
                       kill(RunningProcess->data->child_id,SIGCONT);
                    }else{
                        printf("RunningProcessPrio: %d\n",RunningProcess->data->priority);
                        if(RunningProcess->data->state==2){//terminated
                            //printList(&Ready);
                            RemH(&Ready);
                            //printList(&Ready);
                            RunningProcess=Ready.head;
                            if(RunningProcess!=NULL){
                                RunningProcess->data->state=1;//blocked
                                processorState=0;//idle
                                kill(RunningProcess->data->child_id,SIGCONT);
                                RunningProcess->data->state=0;//running
                                processorState=1;//busy
                            }
                        }
                    }
                }else{
                    processorState=0;//idle
                }
                //printf("State: %d\n",RunningProcess->data->state);
                //if(RunningProcess->data->state==2){
                    //printf("hi\n");
                    //RemH(&Ready);
                    //RunningProcess=Ready.head;
                    //if(RunningProcess!=NULL){
                        //kill(RunningProcess->data->process_id,SIGCONT);
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
