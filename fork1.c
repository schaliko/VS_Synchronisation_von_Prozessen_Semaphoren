#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define NUMBER_OF_PROCESSES 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} argument;

void P(int sem_id) {
    struct sembuf semaphore;
    semaphore.sem_num = 0;
    semaphore.sem_op = -1;
    semaphore.sem_flg = 0;
    if (semop(sem_id, &semaphore, 1) == -1) {
        perror("Error in P() operation");
        exit(1);
    }
}

void V(int sem_id) {
    struct sembuf semaphore;
    semaphore.sem_num = 0;
    semaphore.sem_op = 1;
    semaphore.sem_flg = 0;
    if (semop(sem_id, &semaphore, 1) < 0) {
        perror("Error in V() operation");
        exit(1);
    }
}

int main() {
    key_t sem_key;
    int sem_id;

    if ((sem_key = ftok("./semfile.txt", '1')) < 0) {
        perror("Error in ftok");
        exit(1);
    }

    if ((sem_id = semget(sem_key, 1, IPC_CREAT | 0666)) < 0) {
        perror("Error in semget");
        exit(1);
    }

    argument.val = 1;
    if (semctl(sem_id, 0, SETVAL, argument) < 0) {
        perror("Error in semctl");
        exit(1);
    }

    for (int process = 0; process < NUMBER_OF_PROCESSES; process++) {
        switch (fork()) {
            case -1:
                perror("Fork failed");
                exit(1);

            case 0:  // Child process
                printf("Child process %d (PID: %d) started\n", process, getpid());

                for (int i = 0; i < 3; i++) {
                    P(sem_id);
                    printf("Process %d (PID: %d) enters critical section\n", process, getpid());
                    sleep(1);  
                    printf("Process %d (PID: %d) exits critical section\n", process, getpid());
                    V(sem_id);

                    printf("Process %d (PID: %d) enters non-critical section\n", process, getpid());
                    sleep(1);
                    printf("Process %d (PID: %d) exits non-critical section\n", process, getpid());                
}

                exit(0);

            default:
                break;
        }
    }


    printf("Parent process completed\n");
    return 0;
}
