#include "dining_philosophers.h"

// Structure for shared variable, state
struct shm {
    int state[NUM_PHILOSOPHERS];
} *shared_memory;

// Union variable to initialize semaphores
union semun {
    int val;               // Value for SETVAL
    struct semid_ds *buf;  // Buffer for IPC_STAT, IPC_SET
    unsigned short *array; // Array for GETALL, SETALL
    struct seminfo *__buf; // Buffer for IPC_INFO
} semun_var;

int sem_group;                      // Semaphore group id
int shmid;                          // Shared memory id
pid_t child_pids[NUM_PHILOSOPHERS]; // Array to store philosophers PID for kill operation if fork() fails


int main()
{
    // initialize shared memory
    initialize_shared_memory();
    // semaphore initialization
    initialize_semaphores();
    // philosopher simulation using fork()
    simulate_philosophers();
    // wait for all child processes (philosophers) to exit
    while (wait(NULL) > 0);
    // Clean up the shared memory segment and semaphore set
    cleanup_resources();
    // indicate end of main
    printf("End of main.\n");
    return 0;
}

void initialize_shared_memory()
{
    // create a shared memory section
    shmid = shmget(IPC_PRIVATE, sizeof(*shared_memory), IPC_CREAT | 0666);
    if (shmid == -1) {
        fprintf(stderr, "error in creating shared memory section\n");
        exit(-1);
    } else {
        printf("Memory attached at shmid %d\n", shmid);
    }

    // attach shared memory
    shared_memory = (struct shm *)shmat(shmid, NULL, 0);
    if (shared_memory == (struct shm *)-1) {
        fprintf(stderr, "Shmat failed\n");
        exit(-1);
    } else {
        printf("Shmat succeed\n");
    }

    // assign value to state variable in shared memory
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        shared_memory->state[i] = THINKING;
    }
}

void initialize_semaphores()
{
    // create a group of NUM_PHILOSOPHERS + 1 semaphores
    sem_group = semget(IPC_PRIVATE, NUM_PHILOSOPHERS + 1, IPC_CREAT | 0666);
    if (sem_group == -1) {
        fprintf(stderr, "error in creating semaphore group\n");
        exit(-1);
    } else {
        printf("Semaphores group id: %d\n", sem_group);
    }

    semun_var.array = (unsigned short *)calloc(NUM_PHILOSOPHERS + 1, sizeof(short));
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        semun_var.array[i] = 0; // for pholosophers
    }
    semun_var.array[NUM_PHILOSOPHERS] = 1; // for mutex

    if (semctl(sem_group, 0, SETALL, semun_var) == -1) {
        fprintf(stderr, "error setting semaphore values");
        exit(-1);
    }
}

void simulate_philosophers()
{
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // child process
            philosopher(i);
            exit(0);
        } else if (pid == -1) {
            // print error message
            fprintf(stderr, "error in creating philosophers\n");
            printf("terminating the process...\n");

            // send SIGTERM with kill command to philosophers already created
            for (int j = 0; j < i; j++) {
                if ((kill(child_pids[j], SIGTERM)) == -1) {
                    // print error message if kill wasn't succesful for some reason
                    fprintf(stderr, "error in sending kill comand to philosopher[%d]\n", child_pids[j]);
                }
            }
            // terminate with exit code -1
            exit(-1);
        } else {
            // store PID of philosopher i in an array
            child_pids[i] = pid;
        }

        sleep(1);
    }
}

void philosopher(int i)
{
    // while (true) {
    think(i);
    grab_forks(i);
    eat(i);
    put_away_forks(i);
    // }
}

void grab_forks(int i)
{
    // wait on mutex
    down(NUM_PHILOSOPHERS);
    // change state to hungry
    shared_memory->state[i] = HUNGRY;
    // print a message that philosopher is hungry
    printf("philosopher %d is hungry\n", i);
    // try to grab left and right forks
    test(i);
    // release mutex
    up(NUM_PHILOSOPHERS);
    // wait until forks are available
    down(i);
}

void put_away_forks(int i)
{
    // wait on mutex
    down(NUM_PHILOSOPHERS);
    // change state to thinking
    shared_memory->state[i] = THINKING;
    // check for left and right neighbors
    test(LEFT);
    test(RIGHT);
    // release mutex
    up(NUM_PHILOSOPHERS);
}

void test(int i)
{
    if (shared_memory->state[i] == HUNGRY &&
        shared_memory->state[LEFT] != EATING &&
        shared_memory->state[RIGHT] != EATING) {
        
        // change state to eating
        shared_memory->state[i] = EATING;
        // release fork semaphore for this philosopher
        up(i);
    }
}

void up(int sem_num)
{
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = 1;
    sem_buf.sem_flg = SEM_UNDO;

    if (semop(sem_group, &sem_buf, 1) == -1) {
        // indicate that semaphore operatin has failed
        fprintf(stderr, "error in up operation on semaphore %d\n", sem_num);
    }
}

void down(int sem_num)
{
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = -1;
    sem_buf.sem_flg = SEM_UNDO;

    if (semop(sem_group, &sem_buf, 1) == -1) {
        // indicate that semaphore operatin has failed
        fprintf(stderr, "error in down operation on semaphore %d\n", sem_num);
    }
}

void think(int i)
{
    // print philosopher i is thinking
    printf("philosopher %d is thinking\n", i);
}

void eat(int i)
{
    // print philosopher i is eating
    printf("philosopher %d is eating\n", i);
}

void cleanup_resources()
{
    // detach the shared memory segment
    printf("Detaching shared memory...\n");
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "error detaching shared memory segment\n");
    } else {
        printf("Done.\n");
    }

    // clean up the semaphore set
    printf("cleaning up the semaphore set...\n");
    if (semctl(sem_group, 0, IPC_RMID) == -1) {
        fprintf(stderr, "error in cleaning up semaphore set\n");
    } else {
        printf("Done.\n");
    }

    // clean up the shared memory segment
    printf("cleaning up the shared memory segment..\n");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "error cleaning up shared memory segment\n");
    } else {
        printf("Done.\n");
    }
}
