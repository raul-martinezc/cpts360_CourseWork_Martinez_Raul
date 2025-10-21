#include <sys/ipc.h>   // for semget()
#include <sys/sem.h>   // for semget()
#include <sys/stat.h>  // for S_IRWXU

#include "critical_section.h"
#include "syscall_check.h"

static int semaphoreSetId;

void criticalSection_init(void)
{
    SYSCALL_CHECK(semaphoreSetId = semget(IPC_PRIVATE, 1, IPC_CREAT | S_IRWXU));
}

void criticalSection_enter(void)
{
    struct sembuf semaphoreBuffer[2];

    semaphoreBuffer[0].sem_num = 0; // only semaphore in the set
    semaphoreBuffer[0].sem_op  = 0; // wait for semaphore to become zero
    semaphoreBuffer[0].sem_flg = 0; // no special options

    semaphoreBuffer[1].sem_num = 0; // only semaphore in the set
    semaphoreBuffer[1].sem_op  = 1; // increment semaphore by 1
    semaphoreBuffer[1].sem_flg = 0; // no special options

    SYSCALL_CHECK(semop(semaphoreSetId, semaphoreBuffer, 2));
}

void criticalSection_leave(void)
{
    struct sembuf semaphoreBuffer;

    semaphoreBuffer.sem_num =  0; // only semaphore in the set
    semaphoreBuffer.sem_op  = -1; // decrement semaphore by 1
    semaphoreBuffer.sem_flg =  0; // no special options
    SYSCALL_CHECK(semop(semaphoreSetId, &semaphoreBuffer, 1));
}
