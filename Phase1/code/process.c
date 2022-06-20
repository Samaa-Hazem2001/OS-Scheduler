#include "headers.h"

void handler(int signum);

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

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

void down(int sem)
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


/* Modify this file as needed*/
int id;//id from the input
int arrival_t;
int runtime;
int priority_p;//priorty of the process
int remainingtime;
int lastclk;
// bool stopped = false;
int semproc_sch;
bool notterminateup = false;

int main(int agrc, char * argv[])
{
    signal(SIGCONT, handler);
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

    id = atoi(argv[0]);
    priority_p = atoi(argv[1]);
    arrival_t = atoi(argv[2]);
    runtime = atoi(argv[3]);
    remainingtime = runtime;
    printf("ID =  %d // ",id);
    printf("Arrival =  %d // ", arrival_t);
    printf("Runtime =  %d // ", runtime);
    printf("priority =  %d // ", priority_p);

    printf("start new process \n");
    key_t shrem_id;
    shrem_id = ftok("shrem" , 65);
    initClk();
    
    int shremid = shmget(shrem_id, 4096, 0666 |IPC_CREAT );
    int *shrem = (int * )shmat(shremid, (void *)0, 0);

    if (semctl(semproc_sch, 0, SETVAL, semun) == -1 )
    {
        perror("Error in semctl");
        exit(-1);
    }
    *shrem = remainingtime;

    lastclk = getClk();
    kill(getppid(),SIGUSR1);
    //TODO it needs to get the remaining time from somewhere
    //strcpy(remainingtime , (char*)shrem);
    
    bool changeremian = false; 
    while (remainingtime > 0)
    {
        if(notterminateup){
            up(semproc_sch);
            notterminateup = false;
        }
        if( getClk()-1 == lastclk){  
            --remainingtime;
            changeremian = true;
            //printf("process ID %d running and last time = %d and remaining time = %d\n",id,lastclk,remainingtime);   
            printf("process ID %d running and time = %d and remaining time = %d\n",id,getClk(),remainingtime);
            //strcpy((char *)shrem, remainingtime);
            notterminateup = true;
            lastclk = getClk();
            --(*shrem);
        }
        else if(lastclk+1 < getClk()){
            lastclk = getClk();
        }
        // remainingtime = ??;
    }
    //kill(getppid(),SIGINT);
    destroyClk(false);
    
    exit(0);
    return 0;
}


void handler(int signum){
    printf("CHILD PROCESS IN CONTUNIO HANDLER AND CLOCK = %d\n",getClk());
    lastclk = getClk();
    notterminateup = false;
}