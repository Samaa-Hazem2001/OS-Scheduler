#include "headers.h"
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


void clearResources(int);

typedef struct process 
{
    int ID;
    int ArrivalTime;
    int RunTime;
    int Priority;
    int memorySize;
}Process;


struct msgbuff
{
    long mtype;
    Process process_data;
};

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.

    //knowing the num of processes
    int m=0; //size of array
    FILE* filePointer;
    int bufferLength = 255;
    char buffer[bufferLength]; /* not ISO 90 compatible */

    filePointer = fopen("processes.txt", "r");

    char c;
//     fscanf(filePointer, "%c", &c);
//    printf(" C = %c \n",c);
//     if (c != '#')
//         m++;
    while(fgets(buffer, bufferLength, filePointer)) {
        //fscanf(filePointer, "%c", &c);
        //if (c != '#' )//&& c != "\n")
        if(buffer[0] != '#' && buffer[0]  != '\n')
        {
            m++;
        }
        // printf(" C = %c \n",c);
    }
    //m--;
    fclose(filePointer);

    //reading the input file
    char *filename = "processes.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }

   
    int ch;
    char dummy;
   

  
    
    // int *ID;
    // ID = (int *)malloc(m * sizeof(int));

    // int *Arrival;
    // Arrival = (int *)malloc(m * sizeof(int));

    // int *RunTime;
    // RunTime = (int *)malloc(m * sizeof(int));

    // int *Priority;
    // Priority = (int *)malloc(m * sizeof(int));

    Process* P = (Process*)malloc(m*sizeof(Process));

    
    //fgets(buffer, bufferLength, filePointer);   //skip one line

      for (int i = 0; i < m; i++)
    {
       
            fscanf(fp, "%c", &dummy);
            if(dummy == '#')
            {
                fgets(buffer, bufferLength, filePointer);   //skip one line
                i--;
                continue;
            }
            else
                ungetc(dummy,fp);
            fscanf(fp, "%d", &ch);
        
           // ID[i] = ch;
            P[i].ID = ch;

            fscanf(fp, "%d", &ch);
            //Arrival[i] = ch;

            P[i].ArrivalTime = ch;

            fscanf(fp, "%d", &ch);
            //RunTime[i] = ch;

            P[i].RunTime = ch;

            fscanf(fp, "%d", &ch);
            //Priority[i] = ch;

            P[i].Priority = ch;

            fscanf(fp, "%d", &ch);
            //Priority[i] = ch;

            P[i].memorySize = ch;


        fgets(buffer, bufferLength, filePointer);
       
    }

    fclose(fp);

    
     for (int i=0; i<m; i++)
    {
        // printf("For Process  %d\n", i+1);
        // printf("ID =  %d // ", ID[i]);
        // printf("Arrival =  %d // ", Arrival[i]);
        // printf("Runtime =  %d // ", RunTime[i]);
        // printf("priority =  %d\n", Priority[i]);

        printf("For Process  %d\n", i+1);
        printf("ID =  %d // ", P[i].ID);
        printf("Arrival =  %d // ", P[i].ArrivalTime);
        printf("Runtime =  %d // ", P[i].RunTime);
        printf("priority =  %d\n", P[i].Priority);
        printf("memory =  %d\n", P[i].memorySize);
    }

   // printf("M = %d \n",m);

    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.


    int  q = -1; //Quantam

    int algorithm;   // 1 --> HPF
                     // 2 --> SRTN
                     // 3 --> RR

    bool choosed = false;                 
    key_t key_id;
    int msgq_id, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    
    printf("Choose an algorithm by selecting a number \n");
    printf("1. Non-preemptive Highest Priority First (HPF). \n");
    printf("2. Shortest Remaining time Next (SRTN). \n");
    printf("3. Round Robin (RR). \n");

    while (!choosed)
    {
        scanf("%d", &algorithm);
       // printf("algorithm = %s \n",algorithm);
        if (algorithm == 1)
        {
            printf("You choosed HPF \n");
            choosed = true;
        }

        else if (algorithm == 2)
        {
            printf("You choosed SRTN \n");
            choosed = true;
        }

        else if (algorithm == 3)
        {
            printf("You choosed RR \n");
            printf("Please enter the Quantam Q : ");
            scanf("%d", &q);
            printf("\n");
            choosed = true;
        }
        else
        {
            printf("You entered invalid value,please enter 1 or 2 or 3 only \n");
        }
    }

    
    char Q[5];

    char algo[5];

    char numofprocesses[5];

    // convert 123 to string [buf]
  //  itoa(q, Q, 10);

    sprintf(Q,"%d",q);

    sprintf(algo,"%d",algorithm);

    sprintf(numofprocesses,"%d",m);

    //itoa(algorithm, algo, 10);

    Q[4] = '\0';

    algo[4] = '\0';

    numofprocesses[4] = '\0';

    // 3. Initiate and create the scheduler and clock processes.

    int pid = fork();
    if (pid == 0)
    {
       char* argv[] = { "jim", "jams", NULL };
       char* envp[] = { "some", "environment", NULL };
       if (execve("./clk.out", argv, envp) == -1)
            perror("Could not execve"); 
    }
    
    int pid2 = fork();
    if (pid2 == 0)
    {
       char* argv[] = { algo, Q, numofprocesses , NULL };
       char* envp[] = { "some", "environment", NULL };
       if (execve("./scheduler.out", argv, envp) == -1)
            perror("Could not execve"); 
    }

    // 4. Use this function after creating the clock process to initialize clock

    

    initClk();

   
    // To get time use this
        int x=-1;
    //    while (1)
    //   {
    //       // sleep(1);
    //        if (x != getClk())
    //        {
    //            x = getClk();
    //            printf("current time is %d\n", x);
    //        }
            
    //    }
    
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.



    struct msgbuff message;
    message.mtype = 1; 
    
    for(int i = 0; i < m; i++)
    {
        message.process_data = P[i];
        while(P[i].ArrivalTime != getClk())
        {
            if (x != getClk())
            {
                x = getClk();
               // printf("current time is %d\n", x);
            }
        }
        printf("current time is %d\n", getClk());
        send_val = msgsnd(msgq_id, &message, sizeof(message.process_data), IPC_NOWAIT);

        if (send_val == -1)
            perror("Errror in send");
    }
    // sleep(5);
    // kill(pid2,SIGCHLD);
    // sleep(2);
    waitpid(pid,NULL,0);
    //sleep(3);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    //wait(NULL);
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
