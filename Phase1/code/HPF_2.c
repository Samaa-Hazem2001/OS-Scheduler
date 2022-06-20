#include <stdio.h>
#include <stdlib.h>
//---//
//signals
#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
//---//
//schedular
#include "headers.h"
//----//
//msg queue
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>

//a global variable
bool no_more_proc = false;
//struct of the process information delivered by the process generator
struct proc_info //process entered ingormation
{
    int id;//id from the input
    int arrival_t;
    int runtime;
    int priority_p;//priorty of the process
};
//-----------------------------------------------------------------------------//
//struct of msgbuff between schedular and process generator
struct msgbuff
{
    long mtype;
    struct proc_info m_info_obj;//obj not a pointer
};

//--------------------------------------------------------------------//
//struct of the process information in PCB
//cosist of :an obj of proc_info struct + some additional info belong to PCB logic
struct proc_block
{
    struct proc_info *info_obj;
    //bool waiting;
    //int  execution_t;
    int start_t;
    int end_t;
    int remaining_t;
    int proc_os_id;//id of the getpid or from the fork result
    //int waiting_t;
};
// struct proc_block *block_obj = (struct proc_block *)malloc(sizeof(struct proc_block ));
//--------------------------------------------------------------------//

//priority Queue implementation
typedef struct node {
   //int data;
  // int priority;
    struct proc_block *block_obj;
   struct node* next;
} Node;
Node* newNode(struct proc_block *b_obj) {
   Node* temp = (Node*)malloc(sizeof(Node));
   temp->block_obj = b_obj;
   temp->next = NULL;
   return temp;
}
//peek just return the value of the head node without removing it like Dequeue
struct proc_block * peek(Node** head) {
   return (*head)->block_obj;
}
//DeQueue logic is
    //make the head point to the second node
    //make the next of the first node point to null
    //return the data of the first node which is "block_obj" here
struct proc_block * DeQueue(Node** head) {
   Node* temp = *head;
   (*head) = (*head)->next;
   temp->next = NULL;
   return (*head)->block_obj;
   //free(temp);
}
void InQueue(Node** head, struct proc_block *b_obj,int p)  {
   Node* start = (*head);
   Node* temp = newNode(b_obj);
   //int p =b_obj->info_obj->priority_p;//priority of the new process
   //insert newnode before head node if The head of list has lesser priority ((Hiegher value))than new node
   if ((*head) == NULL || ((*head)->block_obj->info_obj->priority_p) > p) //NOTE:(*head)->priority ">" p => not <
   {
      temp->next = *head;
      (*head) = temp;
   } else {
      while (start->next != NULL &&
      (start->next->block_obj->info_obj->priority_p) < p) {
         start = start->next;
      }
      // Either at the ends of the list
      // or at required position
      temp->next = start->next;
      start->next = temp;
   }
}
// Function to check the queue is empty
int isEmpty(Node** head) {
   return (*head) == NULL;
}
//--------------------------------------------------------------------//

//handle the signal SIGUSR1 that send by the process generator for each new process

//?void handler(int signum); =>no need?
//void handler(int signum)
//{
 // printf("\nOUCH....Signal #%d received\n",signum);

  //get proc_info obj throw msg queue
 // struct proc_block;
  //msgrev

  //fork a process
  //??

  //inqeueue
  //Node* New_Process = newNode(7, 1);

  //? push(&New_Process, 1, 2);


  // signal(SIGUSR1, handler );
//}
//--------------------------------------------------------------------//
//?void handler2(int signum); =>no need?
void handler2(int signum)
{
    no_more_proc = true;
    signal(SIGUSR2, handler2 );
}
//--------------------------------------------------------------------//

//void HPF_algo(Node** head)
void HPF_algo(Node * head,int msgqid)
{
     int  sid ,stat_loc;

    struct proc_block *cur_block_obj;
    struct msgbuff message;
     //no_more_proc : will be true in the handler2
     while(no_more_proc == false || !(isEmpty(head)) )
     {
         if( !(isEmpty(&head)) )
         {
             //do the algo

             //HPF
                //dequeue a node process
                cur_block_obj= DeQueue(&head);
                //pop(&pq);
                //?Later : pop?

                //send signal SIGCONT to this process
                kill(cur_block_obj->proc_os_id, SIGCONT);

                //wait for the runtime
                //wait the process until it send me it is finish
                //while()
                //make sure that :no to check if it is the needed process or not as there will be just one process runing
                cur_block_obj->start_t = getClk();
                sid = wait(&stat_loc);
                if(!(stat_loc & 0x00FF))
                {
                     printf("\nA child with pid %d terminated with exit code %d\n", sid, stat_loc>>8);
                }
                cur_block_obj->end_t = getClk();

                //free the memory location of PCB b_obj
                free(cur_block_obj);

            //check the msg queue ((((WITHOUT WAITing)))))
                readexistmsg(head,1);

            //  int rec_val = msgrcv(msgqid, &message, sizeof(message.m_info_obj), 0, IPC_NOWAIT);//NOTE:IPC_NOWAIT not !IPC_NOWAIT
            // if(rec_val == -1)
            //     perror("Error in receive");
            // else
            // {
            //     //create a PCB to thi process
            //     cur_block_obj = (struct proc_block *)malloc(sizeof(struct proc_block));
            //     cur_block_obj->info_obj = &(message.m_info_obj);//as info_obj is a pointer and m_info_obj is an obj
            //     //some initialization
            //     //NOTE:update it if there is any change is cur_block_obj struct
            //     cur_block_obj->start_t = 0;//?m.s of this initialization
            //     cur_block_obj->end_t = 0;//?m.s of this initialization
            //     cur_block_obj->remaining_t=(message.m_info_obj).runtime;

            //     //fork all
            //     cur_block_obj->proc_os_id= fork();

            //     //send SIGSTOP to all
            //     kill(cur_block_obj->proc_os_id, SIGSTOP);

            //     //inqueue it
            //     InQueue(&head, cur_block_obj);

            // }

                //no need to send signal SIGSTOP to this process

                //decrement the remaining_t of the process by one
                //increment excecution time
                //inc or dec any other necessary proc_block info
                //if the remaining_t>0 =>inqueue the process
                //send signal SIGSTOP to this process
                //repeat the while loop
         }
         else
         {
             //LATERRRRR
             //read all process queue


              //check msg qeueue
              //if it is empty =>(((((WAIT))))(block it)
                waitreadmsgq(head,1);
            // int rec_val = msgrcv(msgqid, &message, sizeof(message.m_info_obj), 0, !IPC_NOWAIT);//NOTE:!IPC_NOWAIT not IPC_NOWAIT
            // if(rec_val == -1)
            //     perror("Error in receive");
            // else
            // {
            //     //create a PCB to thi process
            //     cur_block_obj = (struct proc_block *)malloc(sizeof(struct proc_block));
            //     cur_block_obj->info_obj = &(message.m_info_obj);//as info_obj is a pointer and m_info_obj is an obj
            //     //some initialization
            //     //NOTE:update it if there is any change is cur_block_obj struct
            //     cur_block_obj->start_t = 0;//?m.s of this initialization
            //     cur_block_obj->end_t = 0;//?m.s of this initialization
            //     cur_block_obj->remaining_t=(message.m_info_obj).runtime;

            //     //fork all
            //     cur_block_obj->proc_os_id= fork();

            //     //send SIGSTOP to all
            //     kill(cur_block_obj->proc_os_id, SIGSTOP);

            //     //inqueue it
            //     InQueue(&head, cur_block_obj);

            // }

             //repeat the outer while loop

             //wait until the process generator wakeup you
             //?should the wakeup be in the handler of SIGUSR1 ??

         }
     }
}
//--------------------------------------------------------------------//

int main(int argc, char * argv[])
{
    //create msg queue
    key_t key_id = ftok("keyfile", 65);               //create unique key
    int msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    // struct msgbuff message;

    //intialize th clk
    initClk();

    //SIGUSR2
    //bool no_more_proc = false;
    signal (SIGUSR2, handler2);

    //LATER:
    //handle SIGCHLD
    //signal (SIGCHLD, handler3);
   // bool proc_finish= false;
    //signal (SIGUSR1, handler);

    //head pointer of the queue
    Node* head =NULL;

    //choose the algo
    //LATER
    HPF_algo(head,msgq_id);

    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    destroyClk(true);
}

//--------------------------------------------------------------------//
//Later :check the consept of handling the no_more_proc in SIGUSR2


































