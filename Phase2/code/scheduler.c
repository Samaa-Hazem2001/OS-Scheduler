#include "headers.h"
#include <math.h>
#include <errno.h>
bool child_finish=false;

int clk = -1;
int slot;
int algo;
int numofprocesses = 0;
int numofrecievedproc = 0;
int clock = 0;
bool end = false;

int child = 1;

bool stopped = false;
bool started = false;

bool terminated = false;

key_t shrem_id;
int shremid ;
int *shrem;
key_t key_id;
int rec_val, msgq_id;

int semproc_sch;
FILE *fp;
FILE *pr;
FILE *mr;
double totalRunTime = 0;
double totalWta = 0;
double totalWaiting = 0;
double totalWtaSquare = 0;
void handler(int signum);
void rrhandler(int signum);
void handler2(int signum);
void created(int signum);
void handler3(int signum);


int ID_global;

typedef struct proc_info //process entered ingormation
{
    int id;//id from the input
    int arrival_t;
    int runtime;
    int priority_p;//priorty of the process
    int memorySize;
    //bool lastprocess;
    //bool terminate;

    int paused_t;
    int wait_t ;
}proc_info;

typedef struct proc_block
{
    
    //bool waiting;
    //int  execution_t;
    int priority;
    int start_t;
    int end_t;
    int remaining_t;
    int proc_os_id;//id of the getpid or from the fork result
    //int waiting_t;
    proc_info info_obj;
}proc_block;
struct msgbuff
{
    long mtype;
    struct proc_info process_data;
};

typedef struct node {
   //int data;
  // int priority;
    struct proc_block block_obj;
    struct node* next;
}
Node;

Node* mainhead = NULL;
Node* waiting_queue = NULL;

Node* runprocess;
Node* newNode(struct proc_block b_obj) {
   Node* temp = (Node*)malloc(sizeof(Node));
   temp->block_obj = b_obj;
   temp->next = NULL;
   return temp;
}
//peek just return the value of the head node without removing it like Dequeue
Node* peek(Node** head) {
   //return (*head)->block_obj.info_obj;
   return *head;
}
//DeQueue logic is
    //make the head point to the second node
    //make the next of the first node point to null
    //return the data of the first node which is "block_obj" here
void DeQueue(Node** head);
void InQueue(Node** head, struct proc_block** b_obj, int p);
void InQueuenode(Node** head, Node* node);
// Function to check the queue is empty
int isEmpty(Node** head) {
   return (*head) == NULL;
}



//////////////////////////////////////////////////////////////////////////// Linked List ////////////////////////////////////////////////////////////////////////////////////////



struct Linkednode {
   int size;
   int processID;
   int actualSize;
   bool taken ;
   bool right[7];
   struct Linkednode *next;
   struct Linkednode *prev;
};

//this link always point to first Link
struct Linkednode *Linkedhead = NULL;

//this link always point to last Link 
struct Linkednode *last = NULL;

struct Linkednode *current = NULL;

//is list empty
bool ISEmpty() {
   return Linkedhead == NULL;
}

int length() {
   int length = 0;
   struct Linkednode *current;
	
   for(current = Linkedhead; current != NULL; current = current->next){
      length++;
   }
	
   return length;
}

//display the list in from first to last
void displayForward() {

   //start from the beginning
   struct Linkednode *ptr = Linkedhead;
	
   //navigate till the end of the list
   printf("\n[ ");
	
   while(ptr != NULL) {        
      printf("%d ",ptr->size);
      ptr = ptr->next;
   }
	
   printf(" ]");
}

//display the list from last to first
void displayBackward() {

   //start from the last
   struct Linkednode *ptr = last;
	
   //navigate till the start of the list
   printf("\n[ ");
	
   while(ptr != NULL) {    
	
      //print size
      printf("(%d) ",ptr->size);
		
      //move to next item
      ptr = ptr ->prev;
      
   }
	
}

//insert link at the first location
void insertFirst( int size) {

   //create a link
   struct Linkednode *link = (struct Linkednode*) malloc(sizeof(struct Linkednode));
  // link->key = key;
    link->size = size;
	link->taken = false;
   if(ISEmpty()) {
      //make it the last link
      last = link;
      link->prev = NULL;
      link->next = NULL;
   } else {
      //update first prev link
      Linkedhead->prev = link;
   }

   //point it to old first link
   link->next = Linkedhead;
	
   //point first to new first link
   Linkedhead = link;
}



//delete first item
struct Linkednode* deleteFirst() {

   //save reference to first link
   struct Linkednode *tempLink = Linkedhead;
	
   //if only one link
   if(Linkedhead->next == NULL){
      last = NULL;
   } else {
      Linkedhead->next->prev = NULL;
   }
	
   Linkedhead = Linkedhead->next;
   //return the deleted link
   return tempLink;
}

//delete link at the last location

struct Linkednode* deleteLast() {
   //save reference to last link
   struct Linkednode *tempLink = last;
	
   //if only one link
   if(Linkedhead->next == NULL) {
      Linkedhead = NULL;
   } else {
      last->prev->next = NULL;
   }
	
   last = last->prev;
	
   //return the deleted link
   return tempLink;
}

//delete a link with given key

void delete(struct Linkednode *  n) {

   //start from the first link
   struct Linkednode* current = n;


   //found a match, update the link
   if(current == Linkedhead) {
      //change first to point to next link
      Linkedhead = Linkedhead->next;
   } else {
      //bypass the current link
      current->prev->next = current->next;
   }    

   if(current == last) {
      //change last to point to prev link
      last = current->prev;
   } else {
      current->next->prev = current->prev;
   }
	
   free(current);
}

bool insertAfter(struct Linkednode * n, int size) {
   //start from the first link
   struct Linkednode *current = n; 
	
   //if list is empty
   if(Linkedhead == NULL) {
      return false;
   }


   struct Linkednode *newLink = (struct Linkednode*) malloc(sizeof(struct Linkednode));
  
   newLink->size = size;
    newLink->taken = false;

   if(current == last) {
      newLink->next = NULL; 
      last = newLink; 
   } else {
      newLink->next = current->next;         
      current->next->prev = newLink;
   }
	
   newLink->prev = current; 
   current->next = newLink; 
   return true; 
}



///////////////////////////////////////////////////////////////////////////// Memory //////////////////////////////////////////////////////////////////////////////////


int getMemoryI (int pid , struct Linkednode* root)
{
    int fullLeft = 0;
    while (root->processID != pid)
    {
        fullLeft += root->size;
        root = root->next;
    }

    return fullLeft;
}


int getMemoryJ (int pid , struct Linkednode* root)
{
    int fullLeft = 0;
    while (root->processID != pid)
    {
        fullLeft += root->size;
        root = root->next;
    }
    fullLeft += root->size;
    fullLeft--;
    return fullLeft;
}




bool AddToMemory(struct Linkednode* root,  int proc_id, int size)
{
    while(root != NULL)
    {
        printf("in while addtomemory\n");
        if(root->taken != true)
        {
            if(size <= root->size)
            {
                if(root->size != 8) ///////minimum memory segment
                {
                    if(size <= root->size/2)
                    {
                        fflush(stdin);
                        insertAfter(root, root->size/2);
                        root->next->right[(int)(log(512)-log((root->size)/2))] = false;
                        root->size = root->size/2;
                        root->right[(int)(log(512)-log(root->size))] = true;
                        printf("before recursion \n");
                        AddToMemory(root, proc_id, size);
                        printf("After recursion \n");
                        return true;
                    }
                }
                root->actualSize = size;
                root->processID = proc_id;
                root->taken =true;
                return true;
            }
        }
        root = root->next;
    }
    return false;
}


void mergeFreeMemorySegments(struct Linkednode* root)
{
    while(root != NULL)
    {
        if(root->size == 1024)
            break;
        printf("in while mergeFreeMemorySegments\n");
        if(root->taken == false)
        {
            printf("con1\n");
            printf("%s\n",root->right[(int)(log(512)-log(root->size))] ? "true": "false");
                                                            printf("%d", root->size);

            if(root->right[(int)(log(512)-log(root->size))] == true)
            {
                fflush(stdin);
                printf("con2\n");
                if(root->next == NULL)
                    break;
                if(root->prev == NULL)
                    printf("con4\n");
                else
                    printf("con5\n");
            fflush(stdin);
            printf("%s\n",(root->next->taken == false) ? "true": "false");
            fflush(stdin);

                if(root->next->taken == false && root->size == root->next->size)
                {
                    delete(root->next);
                    root->size = root->size *2;
                    mergeFreeMemorySegments(Linkedhead);
                    return;
                }
            }
        }
        root = root->next;
    }

}

bool FreeMemory(struct Linkednode* root,  int proc_id)
{
    while(root != NULL)
    {
        if(root->processID == proc_id)
        {
            root->taken =false;
            //fprintf(mr, "At time %d freed %d bytes for process %d from i to j\n",getClk(),root->actualSize,root->processID); 

            int i = getMemoryI(proc_id , Linkedhead);
            int j = getMemoryJ(proc_id , Linkedhead);
            fprintf(mr, "At time %d freed %d bytes for process %d from %d to %d\n",getClk(),root->actualSize,root->processID,i,j); 
           mergeFreeMemorySegments(Linkedhead);
           return true;
        }
        root = root->next;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

int down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -slot;
    p_op.sem_flg = IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1 )
    {
        //perror("Error in down()");
        return 0;
    }
    return 1;
}

void downsrtn(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        //perror("Error in down()");
        return;
    }
}

// void down(int sem)
// {
//     struct sembuf p_op;

//     p_op.sem_num = 0;
//     if(algo == 1)
//         return;
//     if(algo == 2)
//         p_op.sem_op = -1;
//     else if(algo == 3)
//         p_op.sem_op = -slot;
//     p_op.sem_flg = !IPC_NOWAIT;

//     if (semop(sem, &p_op, 1) == -1)
//     {
//         //perror("Error in down()");
//         return;
//     }
// }


void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}

void waitreadmsgq(Node ** head){
    printf("in func.\n");
    bool t = true;
    bool b = true;
    while(b && child_finish==false ){
        struct msgbuff message;
        if(t){
            rec_val = msgrcv(msgq_id, &message, sizeof(message.process_data), 0, !IPC_NOWAIT);
            if (rec_val == -1){
                perror("Error in receive");
            }
            else
            {
                numofrecievedproc++;
                printf("recive with wait\n");
                proc_info info;
                info = message.process_data;
                proc_block * newprocess = (proc_block*)malloc(sizeof(proc_block));
                if (newprocess == NULL) {
                    printf("Memory not allocated.\n");
                }
                else{
                    newprocess->info_obj = info;
                    newprocess->start_t = -1;
                    newprocess->end_t = -1;
                    newprocess->proc_os_id = -1;
                    newprocess->remaining_t = newprocess->info_obj.runtime;
                    if(algo == 1)
                        newprocess->priority=newprocess->info_obj.priority_p;
                    else if(algo == 2)
                        newprocess->priority=newprocess->info_obj.runtime;
                    else if(algo == 3)
                        newprocess->priority = 0;
                    
                    //sleep(5);
                }
                //printf("before add\n");
                // printf("process id = %d   Memory size = %d\n",newprocess->info_obj.id,newprocess->info_obj.memorySize);
                // printf("Taken : %s  Size = %d\n",Linkedhead->taken ? "true" : "false",Linkedhead->size);
                // if(AddToMemory(Linkedhead, newprocess->info_obj.id, newprocess->info_obj.memorySize)){

                // }
                // printf("after add\n");
                // displayForward();
                InQueue(&waiting_queue,&newprocess,newprocess->priority);
                printf("IN func.\n");
                t=false;
                //b=false;
            }
            
        }
        else
        {
            rec_val = msgrcv(msgq_id, &message, sizeof(message.process_data), 0, IPC_NOWAIT);
            if (rec_val == -1){
                //perror("Error in receive");
                printf("Not recive \n");
                b=false;
            }
            else{
                numofrecievedproc++;
                proc_info info;
                info = message.process_data;
                proc_block * newprocess = (proc_block*)malloc(sizeof(proc_block));
                if (newprocess == NULL) {
                    printf("Memory not allocated.\n");
                }
                else
                {
                    printf("recive without wait\n");
                    newprocess->info_obj = info;
                    newprocess->start_t = -1;
                    newprocess->end_t = -1;
                    newprocess->proc_os_id = -1;
                    newprocess->remaining_t = newprocess->info_obj.runtime;
                    if(algo == 1)
                        newprocess->priority=newprocess->info_obj.priority_p;
                    else if(algo == 2)
                        newprocess->priority=newprocess->info_obj.runtime;
                    else if(algo == 3)
                        newprocess->priority = 0;
                    
                    //sleep(5);
                }
                InQueue(&waiting_queue,&newprocess,newprocess->priority);
                t=false;
            }
        }
        if(numofrecievedproc == numofprocesses)
            end = true;
    }
    printf("before add wait\n");
    Node* tempqueue = NULL;
    Node* pro;
    if(!isEmpty(&waiting_queue)){
        pro = peek(&waiting_queue);
    }
    else
        pro = NULL;
    while(pro != NULL){
        DeQueue(&waiting_queue);
        printf("\nprocess id = %d   Memory size = %d\n",pro->block_obj.info_obj.id,pro->block_obj.info_obj.memorySize);
        printf("\nTaken : %s  Size = %d\n",Linkedhead->taken ? "true" : "false",Linkedhead->size);
        if(AddToMemory(Linkedhead,pro->block_obj.info_obj.id,pro->block_obj.info_obj.memorySize)){
            printf("Added to memory\n");
            int i = getMemoryI(pro->block_obj.info_obj.id , Linkedhead);
            int j = getMemoryJ(pro->block_obj.info_obj.id , Linkedhead);
            fprintf(mr, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),pro->block_obj.info_obj.memorySize,pro->block_obj.info_obj.id,i,j); 
            InQueuenode(&mainhead,pro);
            printf("after insert node\n");
            displayForward();
        }
        else{
            printf("No Space in Memory\n");
            InQueuenode(&tempqueue,pro);
        }
        if(!isEmpty(&waiting_queue)){
            pro = peek(&waiting_queue);
        }
        else
            pro = NULL;
    }
    while(!isEmpty(&tempqueue)){
        pro = peek(&tempqueue);
        DeQueue(&tempqueue);
        InQueuenode(&waiting_queue,pro);
    }
    printf("after add wait\n");
   // fclose(mr);
}

void readexistmsg(Node ** head){
    bool t = true;
    while(t){
        struct msgbuff message;
        int rec_val = msgrcv(msgq_id, &message, sizeof(message.process_data), 0, IPC_NOWAIT);
        if (rec_val == -1){
            //perror("Error in receive");
            t=false;
        }
        else{
            numofrecievedproc++;
            proc_info info;
            info = message.process_data;
            proc_block * newprocess = (proc_block*)malloc(sizeof(proc_block));
            if (newprocess == NULL) {
                printf("Memory not allocated.\n");
            }
            else
            {
                newprocess->info_obj = info;
                newprocess->start_t = -1;
                newprocess->end_t = -1;
                newprocess->proc_os_id = -1;
                newprocess->remaining_t = newprocess->info_obj.runtime;
                if(algo == 1)
                    newprocess->priority=newprocess->info_obj.priority_p;
                else if(algo == 2)
                    newprocess->priority=newprocess->info_obj.runtime;
                else if(algo == 3)
                    newprocess->priority = 0;
                
            }
            InQueue(&waiting_queue,&newprocess,newprocess->priority);
        }
        if(numofrecievedproc == numofprocesses)
            end = true;
    }
    printf("before add not wait\n");
    Node* tempqueue = NULL;
    Node* pro;
    if(!isEmpty(&waiting_queue)){
        pro = peek(&waiting_queue);
    }
    else
        pro = NULL;
    while(pro != NULL){
        DeQueue(&waiting_queue);
        printf("\nprocess id = %d   Memory size = %d\n",pro->block_obj.info_obj.id,pro->block_obj.info_obj.memorySize);
        printf("\nTaken : %s  Size = %d\n",Linkedhead->taken ? "true" : "false",Linkedhead->size);
        if(AddToMemory(Linkedhead,pro->block_obj.info_obj.id,pro->block_obj.info_obj.memorySize)){
            printf("Added to memory\n");
            //fprintf(mr, "At time %d allocated %d bytes for process %d from i to j\n",getClk(),pro->block_obj.info_obj.memorySize,pro->block_obj.info_obj.id); 
            int i = getMemoryI(pro->block_obj.info_obj.id , Linkedhead);
            int j = getMemoryJ(pro->block_obj.info_obj.id , Linkedhead);
            fprintf(mr, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),pro->block_obj.info_obj.memorySize,pro->block_obj.info_obj.id,i,j); 
            InQueuenode(&mainhead,pro);
            displayForward();
        }
        else{
            printf("No Space in Memory\n");
            InQueuenode(&tempqueue,pro);
        }
        if(!isEmpty(&waiting_queue)){
            pro = peek(&waiting_queue);
        }
        else
            pro = NULL;
    }
    while(!isEmpty(&tempqueue)){
        pro = peek(&tempqueue);
        DeQueue(&tempqueue);
        InQueuenode(&waiting_queue,pro);
    }
    printf("after add not wait\n");
}

void srtn(Node ** head){
    fp  = fopen ("scheduler.log", "w");
    fprintf(fp, "#At time x process y state arr w total z remain y wait k\n"); 
    while(!((*head)==NULL && end)){
        printf("in while \n");
        if((*head) == NULL && !end){
            printf("first enter\n");
            waitreadmsgq(&mainhead);
        }

        else if(!end){
            readexistmsg(&mainhead);
        }


        printf("start algo\n");
        Node* runprocess = peek(&mainhead);
        DeQueue(&mainhead);
        printf("os id =  %d \n", runprocess->block_obj.proc_os_id);
        if(runprocess->block_obj.proc_os_id == -1){
            printf("cond. 1\n");
            //runprocess->block_obj.start_t = getClk();
            //printf("process id = %d and start at time %d \n",runprocess->block_obj.info_obj.id,runprocess->block_obj.start_t);
            clock = -1;
            char id[5];
            char priority[5];
            char arrival[5];
            char running[5];
            sprintf(id,"%d",runprocess->block_obj.info_obj.id);
            sprintf(priority,"%d",runprocess->block_obj.info_obj.priority_p);
            sprintf(arrival,"%d",runprocess->block_obj.info_obj.arrival_t);
            sprintf(running,"%d",runprocess->block_obj.info_obj.runtime);
            char* argv[] = { id, priority,arrival,running, NULL };
            char* envp[] = { "some", "environment", NULL };
            //started = true;
            int pid = fork();
            if( pid == 0 ){
                printf("I'm Child\n");
                // exit(0);
                if (execve("./process.out", argv, envp) == -1)
                    perror("Could not execve");
            }
            else{
                ID_global = runprocess->block_obj.info_obj.id;
                runprocess->block_obj.proc_os_id=pid;
                //sleep(10);
                *shrem = runprocess->block_obj.remaining_t;
                while(clock == -1);
                runprocess->block_obj.start_t = getClk();
                printf("I'm parent\n");  
                printf("process id = %d and start at time %d \n",runprocess->block_obj.info_obj.id,runprocess->block_obj.start_t);  
                runprocess->block_obj.info_obj.wait_t = 0;
                runprocess->block_obj.info_obj.wait_t += (runprocess->block_obj.start_t-runprocess->block_obj.info_obj.arrival_t);
                fprintf(fp, "At time %d  process %d  started arr %d  total %d  remain %d  wait %d\n",runprocess->block_obj.start_t,runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,runprocess->block_obj.remaining_t,runprocess->block_obj.info_obj.wait_t);  
                totalRunTime +=   runprocess->block_obj.info_obj.runtime;   
            }
        }
        else{
            printf("cond. 2\n");
            ID_global = runprocess->block_obj.info_obj.id;
            started = true;
            kill(runprocess->block_obj.proc_os_id, SIGCONT);
            *shrem = runprocess->block_obj.remaining_t;
            printf("process id = %d and continue at time %d\n",runprocess->block_obj.info_obj.id,(int)getClk());
            runprocess->block_obj.info_obj.wait_t += getClk()-runprocess->block_obj.info_obj.paused_t;
            fprintf(fp,"At time %d  process %d  resumed arr %d  total %d  remain %d  wait %d\n" , (int)getClk(),runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,*shrem,runprocess->block_obj.info_obj.wait_t);
        }
        clock = getClk();
        int id = runprocess->block_obj.info_obj.id;
        bool procwithsmlremain = false;
        printf("before end cond.\n");
        // this code wait for process if no processes would come from process generators but it crash in some case so it commented
        // if(end){
        //     //wait(NULL);

        //     printf("before sleep \n");
        //     waitpid(runprocess->block_obj.proc_os_id,NULL,0);
        //     //sleep(runprocess->block_obj.info_obj.runtime + 2);
        //     printf("after sleep \n");
        //     child_finish = true;
        // }
        // else
        {
            printf("in else while loop\n");
            while(!child_finish && !procwithsmlremain){
                waitreadmsgq(&mainhead);
                if(!isEmpty(&mainhead)){
                    if(id != mainhead->block_obj.info_obj.id){
                        printf("process id = %d\n",runprocess->block_obj.info_obj.id);
                        printf("Initial clock = %d\n",clock);
                        printf("current clock = %d\n",getClk());
                        printf("Initial remianing = %d\n",runprocess->block_obj.priority);
                        printf("current remianing = %d\n",*shrem);
                        while(getClk()-clock != runprocess->block_obj.priority - *shrem){
                            //printf("in while");
                        }
                        runprocess->block_obj.remaining_t = *shrem;
                        runprocess->block_obj.priority = runprocess->block_obj.remaining_t;
                        clock = getClk();
                        if(mainhead->block_obj.remaining_t < runprocess->block_obj.remaining_t){
                            procwithsmlremain = true;
                        }
                        else{
                            id=mainhead->block_obj.info_obj.id;
                        }
                    }
                }
            }
        }
        printf("after end cond.\n");
        if(child_finish){
            runprocess->block_obj.end_t = getClk();
            printf("process id = %d and end at time %d\n",runprocess->block_obj.info_obj.id,getClk());
            float TA = runprocess->block_obj.end_t-runprocess->block_obj.info_obj.arrival_t;
            fprintf(fp,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.3g\n",getClk() , runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,*shrem,runprocess->block_obj.info_obj.wait_t,(int)TA,(TA/runprocess->block_obj.info_obj.runtime));
            totalWta += (TA/runprocess->block_obj.info_obj.runtime);
            totalWaiting += runprocess->block_obj.info_obj.wait_t;
            totalWtaSquare += ((double)(TA/runprocess->block_obj.info_obj.runtime))*((double)(TA/runprocess->block_obj.info_obj.runtime));
            free(runprocess);
        }
        else if(procwithsmlremain){

            while(getClk()-clock != *shrem - runprocess->block_obj.remaining_t);
            stopped = true;
            kill(runprocess->block_obj.proc_os_id, SIGSTOP);
            printf("process id = %d and stop at time %d\n",runprocess->block_obj.info_obj.id,getClk());
            runprocess->block_obj.info_obj.paused_t = getClk();
            fprintf(fp,"At time %d process %d stopped arr %d total %d remain %d wait %d\n" , getClk() , runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,*shrem,runprocess->block_obj.info_obj.wait_t);
            runprocess->block_obj.remaining_t = *shrem;
            runprocess->block_obj.priority = runprocess->block_obj.remaining_t;
            printf("remaining time before stop = %d\n",runprocess->block_obj.remaining_t);
            InQueuenode(&mainhead,runprocess);
        }
        child_finish =false;
        printf("before end termination.\n");


    }
    fclose(fp);
    double cpu = (totalRunTime/getClk())*100;
     double avgWta = totalWta/(double)numofprocesses;
     double avgWaiting = totalWaiting/(double)numofprocesses;
     double stdSqure = ((totalWtaSquare-numofprocesses*avgWta*avgWta)/(numofprocesses-1));
     pr  = fopen ("scheduler.perf", "w");
     fprintf(pr,"CPU utilization = %.2f %c \n",cpu , '%');
     fprintf(pr,"Avg WTA = %.2f \n",avgWta);
     fprintf(pr,"Avg Waiting = %.2f \n",avgWaiting );
     fprintf(pr,"Std WTA = %.2f \n",sqrt(stdSqure));
     fclose(pr);

}

void rr(Node ** head){
    fp  = fopen ("scheduler.log", "w");
    fprintf(fp, "#At time x process y state arr w total z remain y wait k\n"); 
    //signal (SIGINT, rrhandler);
    while(!((*head)==NULL && end)){

        if((*head) == NULL && !end){
            printf("first enter\n");
            waitreadmsgq(&mainhead);
        }

        else if(!end){
            readexistmsg(&mainhead);
        }


        printf("start algo\n");
        runprocess = peek(&mainhead);
        DeQueue(&mainhead);
        printf("os id =  %d \n", runprocess->block_obj.proc_os_id);
        if(runprocess->block_obj.proc_os_id == -1){
            printf("cond. 1\n");
            //runprocess->block_obj.start_t = getClk();
            //printf("process id = %d and start at time %d \n",runprocess->block_obj.info_obj.id,runprocess->block_obj.start_t);
            clock = -1;
            char id[5];
            char priority[5];
            char arrival[5];
            char running[5];
            sprintf(id,"%d",runprocess->block_obj.info_obj.id);
            sprintf(priority,"%d",runprocess->block_obj.info_obj.priority_p);
            sprintf(arrival,"%d",runprocess->block_obj.info_obj.arrival_t);
            sprintf(running,"%d",runprocess->block_obj.info_obj.runtime);
            char* argv[] = { id, priority,arrival,running, NULL };
            char* envp[] = { "some", "environment", NULL };
            int pid = fork();
            if( pid == 0 ){
                printf("I'm Child\n");
                // exit(0);
                if (execve("./process.out", argv, envp) == -1)
                    perror("Could not execve");
            }
            else{
                runprocess->block_obj.proc_os_id=pid;
                *shrem = runprocess->block_obj.remaining_t;
                while(clock == -1);
                runprocess->block_obj.start_t = getClk();
                // sleep(10);
                printf("I'm parent\n");            
                printf("process id = %d and start at time %d \n",runprocess->block_obj.info_obj.id,runprocess->block_obj.start_t);  
                runprocess->block_obj.info_obj.wait_t = 0;
                runprocess->block_obj.info_obj.wait_t += (runprocess->block_obj.start_t-runprocess->block_obj.info_obj.arrival_t);
                printf("hello there 1\n");  
                fprintf(fp, "At time %d process %d started arr %d total %d remain %d wait %d \n",runprocess->block_obj.start_t,runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,runprocess->block_obj.info_obj.runtime,runprocess->block_obj.info_obj.wait_t);
                totalRunTime += runprocess->block_obj.info_obj.runtime;
                ID_global = runprocess->block_obj.info_obj.id;
                printf("hello there\n");  
            }
        }
        else{
            printf("cond. 2\n");
            *shrem = runprocess->block_obj.remaining_t;
            started = true;
            kill(runprocess->block_obj.proc_os_id, SIGCONT);
            ID_global = runprocess->block_obj.info_obj.id;
            printf("process id = %d and continue at time %d\n",runprocess->block_obj.info_obj.id,(int)getClk());
            runprocess->block_obj.info_obj.wait_t += getClk()-runprocess->block_obj.info_obj.paused_t;
            fprintf(fp,"At time %d process %d resumed arr %d total %d remain %d wait %d\n" , (int)getClk(),runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,*shrem,runprocess->block_obj.info_obj.wait_t);
        }
        printf("wait for process semaphor \n");
        union Semun semun;
        semun.val = 0;
        semctl(semproc_sch, 0, SETVAL, semun);
        // this code wait for process if no processes would come from process generators but it crash in some case so it commented
        // if((*head)==NULL ){
        //     if(end){
        //         waitpid(runprocess->block_obj.proc_os_id,NULL,0);
        //         //sleep(runprocess->block_obj.info_obj.runtime + 2);
        //         child_finish = true;
        //     }
        //     else{
        //         while((*head)==NULL ){
        //             waitreadmsgq(&mainhead);
        //         }
        //     }
        // }
        while(!down(semproc_sch) && !child_finish){
            if(!end){
                readexistmsg(&mainhead);
            }
        }
        printf("semaphor released \n");
        if(!child_finish){
            stopped = true;
            kill(runprocess->block_obj.proc_os_id, SIGSTOP);
            printf("process id = %d and stop at time %d\n",runprocess->block_obj.info_obj.id,getClk());
            runprocess->block_obj.info_obj.paused_t = getClk();
            runprocess->block_obj.remaining_t = *shrem;
            fprintf(fp,"At time %d process %d stopped arr %d total %d remain %d wait %d\n" , getClk() , runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,*shrem,runprocess->block_obj.info_obj.wait_t);
            if(!end){
                readexistmsg(&mainhead);
            }
            InQueuenode(&mainhead,runprocess); 
        }
        else{
            runprocess->block_obj.end_t = getClk();
            printf("process id = %d and end at time %d\n",runprocess->block_obj.info_obj.id,getClk());
            float TA = runprocess->block_obj.end_t-runprocess->block_obj.info_obj.arrival_t;
            fprintf(fp,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.3g\n",getClk() , runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,*shrem,runprocess->block_obj.info_obj.wait_t,(int)TA,(TA/runprocess->block_obj.info_obj.runtime));
            totalWta += (TA/runprocess->block_obj.info_obj.runtime);
            totalWaiting += runprocess->block_obj.info_obj.wait_t;
            totalWtaSquare += ((double)(TA/runprocess->block_obj.info_obj.runtime))*((double)(TA/runprocess->block_obj.info_obj.runtime));
            free(runprocess);
        }
        child_finish =false;


    }
    fprintf(fp,"time after finish algorithm : %d \n",getClk());
    fclose(fp);
    double cpu = (totalRunTime/getClk())*100;
     double avgWta = totalWta/(double)numofprocesses;
     double avgWaiting = totalWaiting/(double)numofprocesses;
     double stdSqure = ((totalWtaSquare-numofprocesses*avgWta*avgWta)/(numofprocesses-1));
     pr  = fopen ("scheduler.perf", "w");
     fprintf(pr,"CPU utilization = %.2f %c \n",cpu , '%');
     fprintf(pr,"Avg WTA = %.2f \n",avgWta);
     fprintf(pr,"Avg Waiting = %.2f \n",avgWaiting );
     fprintf(pr,"Std WTA = %.2f \n",sqrt(stdSqure));
     fclose(pr);
}


void HPF_algo(Node ** head)//,int msgqid)
{
    fp  = fopen ("scheduler.log", "w");
    
    fprintf(fp, "#At time x process y state arr w total z remain y wait k\n"); 

    struct proc_block *cur_block_obj;
    struct msgbuff message;
     //no_more_proc : will be true in the handler2
     while(end == false || !(isEmpty(head)) )
     {
         if( !(isEmpty(head)) )
         {
             //print_queue(head);
                //dequeue a node process
             Node* runprocess = peek(head);
            DeQueue(head);
            cur_block_obj=&runprocess->block_obj;
            char id[5];
            char priority[5];
            char arrival[5];
            char running[5];
            sprintf(id,"%d",cur_block_obj->info_obj.id);
            sprintf(priority,"%d",cur_block_obj->info_obj.priority_p);
            sprintf(arrival,"%d",cur_block_obj->info_obj.arrival_t);
            sprintf(running,"%d",cur_block_obj->info_obj.runtime);
            char* argv[] = { id, priority,arrival,running, NULL };
            char* envp[] = { "some", "environment", NULL };
            clock = -1;
            int pid = fork();
            if( pid == 0 ){
                // exit(0);
                if (execve("./process.out", argv, envp) == -1)
                    perror("Could not execve");
            }
            else{
                cur_block_obj->proc_os_id=pid;
                ID_global = runprocess->block_obj.info_obj.id;
                // sleep(10);
            }
                //send signal SIGCONT to this process
                while(clock == -1);
                cur_block_obj->start_t = getClk();
                runprocess->block_obj.start_t = getClk();
                runprocess->block_obj.info_obj.wait_t = 0;
                runprocess->block_obj.info_obj.wait_t += (runprocess->block_obj.start_t-runprocess->block_obj.info_obj.arrival_t);                
                fprintf(fp, "At time %d process %d started arr %d total %d remain %d wait %d \n",runprocess->block_obj.start_t,runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,runprocess->block_obj.info_obj.runtime,runprocess->block_obj.start_t-runprocess->block_obj.info_obj.arrival_t); 
                totalRunTime += runprocess->block_obj.info_obj.runtime;
                printf("\n---------------current clk =%d , before ending the process with id = %d---------------\n",getClk(),cur_block_obj->info_obj.id);
                while(child_finish == false)
                {
                    waitreadmsgq(head);
                    //readexistmsg(head);
                }
                cur_block_obj->end_t = getClk();
                runprocess->block_obj.end_t = getClk();
                float TA = runprocess->block_obj.end_t-runprocess->block_obj.info_obj.arrival_t;
                fprintf(fp,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.3g\n",getClk() , runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,*shrem,runprocess->block_obj.info_obj.wait_t,(int)TA,(TA/runprocess->block_obj.info_obj.runtime));
                totalWta += (TA/runprocess->block_obj.info_obj.runtime);
                totalWaiting += runprocess->block_obj.info_obj.wait_t;
                totalWtaSquare += ((double)(TA/runprocess->block_obj.info_obj.runtime))*((double)(TA/runprocess->block_obj.info_obj.runtime));
               printf("\n--------------current clk after ending the process = %d-------------\n",getClk());

               //free the memory location of PCB b_obj
               free(cur_block_obj);


                //reset child_finish
                child_finish=false;

                //NOTE:we need to look over if there is any process reached at the same moment that this child process finish
                //as the process may send the signal child before the schedular read the new pricess in the msg queue
                readexistmsg(head);
         }
         else
         {
              //check msg qeueue
              //if it is empty =>(((((WAIT))))(block it)
                waitreadmsgq(head);

         }
     }
     fclose(fp);
     double cpu = (totalRunTime/getClk())*100;
     double avgWta = totalWta/(double)numofprocesses;
     double avgWaiting = totalWaiting/(double)numofprocesses;
     double stdSqure = ((totalWtaSquare-numofprocesses*avgWta*avgWta)/(numofprocesses-1));
     pr  = fopen ("scheduler.perf", "w");
     fprintf(pr,"CPU utilization = %.2f %c \n",cpu , '%');
     fprintf(pr,"Avg WTA = %.2f \n",avgWta);
     fprintf(pr,"Avg Waiting = %.2f \n",avgWaiting );
     fprintf(pr,"Std WTA = %.2f \n",sqrt(stdSqure));
     fclose(pr);

}
//--------------------------------------------------------------------//

int main(int argc, char * argv[])
{
    signal(SIGCHLD,handler3);
    //SIGUSR2
    //bool no_more_proc = false;
    mr  = fopen ("memory.log", "w");
    
    fprintf(mr, "#At time x Allocated y bytes for process z from i to j\n"); 
    insertFirst(1024);       // total memory


    algo = atoi(argv[0]);
    slot = atoi(argv[1]);
    numofprocesses = atoi(argv[2]);

    printf("Algo no. : %d  and slots = %d\n",algo,slot);

    //signal (SIGUSR2, handler2);
    //signal (SIGINT, handler);
    signal (SIGUSR1, created);
    

    union Semun semun;
    key_t proc_sch_id,sch_proc_id;
    proc_sch_id = ftok("proc_sch" , 65);
    semproc_sch = semget(proc_sch_id, 1, 0666 | IPC_CREAT);
    if (semproc_sch == -1 )
    {
        perror("Error in create sem");
        exit(-1);
    }
    semun.val = 0;
    if (semctl(semproc_sch, 0, SETVAL, semun) == -1 )
    {
        perror("Error in semctl");
        exit(-1);
    }
    
    key_t shrem_id;
    shrem_id = ftok("shrem" , 65);
    int shremid = shmget(shrem_id, 4096, 0666 |IPC_CREAT );
    shrem = (int*)shmat(shremid, (void *)0, 0);

    key_id = ftok("keyfile", 65);               //create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    
    initClk();
    printf("before algo.\n");
    if(algo == 1)
        HPF_algo(&mainhead);
    else if(algo == 2)
        srtn(&mainhead);
    else if(algo == 3)
        rr(&mainhead);

    printf("after algo.\n");
    fclose(mr);

    printf("after closing.\n");
    destroyClk(true);
    displayForward();

    
    return 0;
}

void handler(int signum){
    printf("In terminated Handler \n");
    terminated = true;
}

void rrhandler(int signum){
    printf("In terminated Handler \n");
    runprocess->block_obj.end_t = getClk();
    printf("process id = %d and end at time %d\n",runprocess->block_obj.info_obj.id,getClk());
    free(runprocess);
    terminated = true;
}

void created(int signum){
    printf("process created 1\n");
    clock = getClk();
    //int c = getClk();
    //runprocess->block_obj.start_t = getClk();
    //printf("process id = %d and start at time %d \n",runprocess->block_obj.info_obj.id,getClk());
    printf("process created 2\n");
}

void handler2(int signum)
{
    end = true;
    down(semproc_sch);
    signal(SIGUSR2, handler2 );
}
void handler3(int signum)
{
    
    if(!started && !stopped){
        int sid,stat_loc;
        sid = wait(&stat_loc);
        printf("\nnum. enter signal child handler = %d\n",child++);
        printf("\nin Signal Child Handler\n");
        //m.s: we need to using "wait" as it kills the child beside waiting for it 
        fprintf(stderr, "Value of errno: %d\n", errno);
        printf("%d", ID_global);

        displayForward();
        FreeMemory(Linkedhead, ID_global);
        displayForward();
        printf("here");
        ///////////////////////////////////////////////////////////
        printf("before add handler\n");
        Node* tempqueue = NULL;
        Node* pro;
        if(!isEmpty(&waiting_queue)){
            pro = peek(&waiting_queue);
        }
        else
            pro = NULL;
        while(pro != NULL){
            DeQueue(&waiting_queue);
            printf("\nprocess id = %d   Memory size = %d\n",pro->block_obj.info_obj.id,pro->block_obj.info_obj.memorySize);
            printf("\nTaken : %s  Size = %d\n",Linkedhead->taken ? "true" : "false",Linkedhead->size);
            if(AddToMemory(Linkedhead,pro->block_obj.info_obj.id,pro->block_obj.info_obj.memorySize)){
                InQueuenode(&mainhead,pro);
              //  fprintf(mr, "At time %d allocated %d bytes for process %d from i to j\n",getClk(),pro->block_obj.info_obj.memorySize,pro->block_obj.info_obj.id); 

              int i = getMemoryI(pro->block_obj.info_obj.id , Linkedhead);
            int j = getMemoryJ(pro->block_obj.info_obj.id , Linkedhead);
            fprintf(mr, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),pro->block_obj.info_obj.memorySize,pro->block_obj.info_obj.id,i,j); 
                displayForward();
            }
            else{
                InQueuenode(&tempqueue,pro);
            }
            if(!isEmpty(&waiting_queue)){
                pro = peek(&waiting_queue);
            }
            else
                pro = NULL;
        }
        while(!isEmpty(&tempqueue)){
            pro = peek(&tempqueue);
            DeQueue(&tempqueue);
            InQueuenode(&waiting_queue,pro);
        }
        printf("after add handler\n");
        ///////////////////////////////////////////////////////////
        if(!(stat_loc & 0x00FF))
        {
            printf("\nA child with pid %d terminated with exit code %d\n", sid, stat_loc>>8);
        }
        child_finish=true;
    }
    else{
        if(started){
            printf("\nin Signal Child Handler but process is started or stopped \n");
            started = false;
        }
        else if(stopped){
            printf("\nin Signal Child Handler but process is started or stopped \n");
            stopped = false;
        }
    }
}



void DeQueue(Node** head) {
	Node* temp = *head;
	(*head) = (*head)->next;
	//free(temp);
    //free(temp);
}

void InQueue(Node** head, struct proc_block ** b_obj, int p) {
    printf("inqueue\n");
    printf("befor new node\n");
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->block_obj = **b_obj;
    temp->next = NULL;

    printf("ID =  %d // ",temp->block_obj.info_obj.id);
    printf("Arrival =  %d // ", temp->block_obj.info_obj.arrival_t);
    printf("Runtime =  %d // ", temp->block_obj.info_obj.runtime);
    printf("memory =  %d // ", temp->block_obj.info_obj.memorySize);
    printf("priority =  %d\n", temp->block_obj.info_obj.priority_p);

   printf("after new node\n");
   if (*head == NULL ) //NOTE:(*head)->priority ">" p => not <
   {
       printf("cond. 1 \n");
      // temp->next = NULL;
        *head = temp;

   }
   else 
   {    
          Node* start = (*head);
       if((*head)->block_obj.priority > p){
            printf("cond. 2 \n");
            temp->next = *head;
            (*head) = temp;
        }
        else {
            printf("cond. else \n");
            if(start->next ==NULL){
                printf("Next = Null\n");
            }
            while (start->next != NULL &&
            (start->next->block_obj.priority) <= p) {
                start = start->next;
                printf("loop 1 \n");
            }
            // Either at the ends of the list
            // or at required position
            temp->next = start->next;
            start->next = temp;
            
        }
   }
   printf("finish inqueue\n");
   return;
}
void InQueuenode(Node** head, Node* node){
    Node* temp = node;
    int p = temp->block_obj.priority;
    if (*head == NULL ) //NOTE:(*head)->priority ">" p => not <
   {
        temp->next = NULL;
        *head = temp;      
   }
   else 
   {    
        Node* start = (*head);
        if((*head)->block_obj.priority > p){
            temp->next = *head;
            (*head) = temp;
        }
        else {
            if(start->next ==NULL){
                printf("Next = Null\n");
            }
            while (start->next != NULL && ((start->next->block_obj.priority) <= p) ) {
                //printf("current id = %d  and next id = %d \n",start->block_obj.info_obj.id,start->next->block_obj.info_obj.id);
                start = start->next;
                //printf("in while\n");
            }
            // Either at the ends of the list
            // or at required position
            printf("after while\n");
            temp->next = start->next;
            start->next = temp;
            
        }
   }
   printf("finish inqueue node\n");
   return;
}
