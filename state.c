#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void *func1(void *arg)
{
    struct sembuf sops[2] = {0,-1,0,1,1,0};
    while(1)
    {
        semop(*(int*)arg,&sops[0],1);
        write(STDOUT_FILENO,"A---",strlen("C---"));
        sleep(1);
        semop(*(int*)arg,&sops[1],1);
    }
}
void *func2(void *arg)
{
    struct sembuf sops[2] = {1,-1,0,2,1,0};
    while(1)
    {
        semop(*(int*)arg,&sops[0],1);
        write(STDOUT_FILENO,"B---",strlen("B---"));
        sleep(1);
        semop(*(int*)arg,&sops[1],1);
    }
}
void *func3(void *arg)
{
    struct sembuf sops[2] = {2,-1,0,0,1,0};
    while(1)
    {
        semop(*(int*)arg,&sops[0],1);
        write(STDOUT_FILENO,"C---",strlen("C---"));
        sleep(1);
        semop(*(int*)arg,&sops[1],1);
    }
}
int main()
{
    int semid,ret;
    pthread_t thr1,thr2,thr3;
    semid = semget(IPC_PRIVATE,3,IPC_CREAT|0666);
    if(semid == -1)
    {
        perror("semget error");
        exit(-1);
    }
    union semun arg;
    arg.val = 1;
    ret = semctl(semid,0,SETVAL,arg);
    if(ret == -1)
    {
        perror("semctl error");
        exit(-1);
    }
    arg.val = 0;
    ret = semctl(semid,1,SETVAL,arg);
    if(ret == -1)
    {
        perror("semctl error");
        exit(-1);
    }
    ret = semctl(semid,2,SETVAL,arg);
    if(ret == -1)
    {
        perror("semctl error");
        exit(-1);
    }
    pthread_create(&thr1,NULL,func1,&semid);
    pthread_create(&thr2,NULL,func2,&semid);
    pthread_create(&thr3,NULL,func3,&semid);
    pthread_join(thr1,NULL);
    pthread_join(thr2,NULL);
    pthread_join(thr3,NULL);
    return 0;
}