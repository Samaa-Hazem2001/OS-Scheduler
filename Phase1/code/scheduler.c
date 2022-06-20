#include "headers.h"
#include <math.h>
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
double totalRunTime = 0;
double totalWta = 0;
double totalWaiting = 0;
double totalWtaSquare = 0;
void handler(int signum);
void rrhandler(int signum);
void handler2(int signum);
void created(int signum);
void handler3(int signum);

typedef struct proc_info //process entered ingormation
{
    int id;//id from the input
    int arrival_t;
    int runtime;
    int priority_p;//priorty of the process
    int paused_t;
    int wait_t ;
    //bool lastprocess;
    //bool terminate;
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
                InQueue(&mainhead,&newprocess,newprocess->priority);
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
                InQueue(&mainhead,&newprocess,newprocess->priority);
                t=false;
            }
        }
        if(numofrecievedproc == numofprocesses)
            end = true;
    }
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
                
                //sleep(5);
            }
            InQueue(&mainhead,&newprocess,newprocess->priority);
        }
        if(numofrecievedproc == numofprocesses)
            end = true;
    }
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
        {
            printf("in else while loop\n");
            while(!child_finish && !procwithsmlremain){
                waitreadmsgq(&mainhead);
                if(!isEmpty(&mainhead)){
                    if(id != mainhead->block_obj.info_obj.id){
                        printf("Initial clock = %d\n",clock);
                        printf("current clock = %d\n",getClk());
                        printf("Initial remianing = %d\n",runprocess->block_obj.remaining_t);
                        printf("current remianing = %d\n",*shrem);
                        while(getClk()-clock != runprocess->block_obj.remaining_t - *shrem);
                        runprocess->block_obj.remaining_t = *shrem;
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
                fprintf(fp, "At time %d process %d started arr %d total %d remain %d wait %d \n",runprocess->block_obj.start_t,runprocess->block_obj.info_obj.id,runprocess->block_obj.info_obj.arrival_t,runprocess->block_obj.info_obj.runtime,runprocess->block_obj.info_obj.runtime,runprocess->block_obj.info_obj.wait_t);
                totalRunTime += runprocess->block_obj.info_obj.runtime;
            }
        }
        else{
            printf("cond. 2\n");
            *shrem = runprocess->block_obj.remaining_t;
            started = true;
            kill(runprocess->block_obj.proc_os_id, SIGCONT);
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
                //samaa: printf("I'm Child\n");
                // exit(0);
                if (execve("./process.out", argv, envp) == -1)
                    perror("Could not execve");
            }
            else{
                cur_block_obj->proc_os_id=pid;
                // sleep(10);
                //samaa: printf("I'm parent\n");
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
               //samaa:
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
    destroyClk(true);
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
        printf("\nnum. enter signal child handler = %d\n",child++);
        printf("\nin Signal Child Handler\n");
        //m.s: we need to using "wait" as it kills the child beside waiting for it
        int sid,stat_loc;
        sid = wait(&stat_loc);
        if(!(stat_loc & 0x00FF))
        {
            //samaa:
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
    printf("priority =  %d\n", temp->block_obj.info_obj.priority_p);

   printf("after new node\n");
   if (mainhead == NULL ) //NOTE:(*head)->priority ">" p => not <
   {
       printf("cond. 1 \n");
        mainhead = temp;
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
   return;
}
void InQueuenode(Node** head, Node* node){
    Node* temp = node;
    int p = temp->block_obj.priority;
    if (mainhead == NULL ) //NOTE:(*head)->priority ">" p => not <
   {
        mainhead = temp;
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
            }
            while (start->next != NULL &&
            (start->next->block_obj.priority) <= p) {
                start = start->next;
            }
            // Either at the ends of the list
            // or at required position
            temp->next = start->next;
            start->next = temp;
            
        }
   }
   return;
}