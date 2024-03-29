#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <semaphore.h>
#include <string.h>

#define SHM_ID "/pids_shared_memory"
#define SEM_PATH_1 "/pid_path_1"
#define SEM_PATH_2 "/pid_path_2"
#define MAX_PIDS 5 

pid_t *pid_list;

int main() {
    // Create or open a shared memory object for PIDs
    int shm_fd_pids = shm_open(SHM_ID, O_RDWR, 0666);
    printf("The shared memory is created \n");
    if (shm_fd_pids == -1) {
        perror("shm_open failed");
        return -1;
    }

    // Map the shared memory segment into the address space for PIDs
    pid_list = (pid_t*)mmap(NULL, MAX_PIDS * sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_pids, 0);
    if (pid_list == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Open semaphores
    sem_t *sem_id1 = sem_open(SEM_PATH_1, 0);
    sem_t *sem_id2 = sem_open(SEM_PATH_2, 0);

    /* Wait for Master to write */
    printf(" Watchdog is on ....");
    printf("I am waiting for the semaphore \n");

    sem_wait(sem_id2);
    printf("data read \n");

    // Display the received PIDs
    for (int i = 0; i < MAX_PIDS; i++) {
        printf("Received PID: %d\n", (int)pid_list[i]);
        fflush(stdout);
    }

    /* Restart writer */
    sem_post(sem_id1);

    const char *filename = "/home/yaz/Downloads/ARP_Drone_obs edited/Logs/watchdog_log.txt";
    bool brk = false;

    while (1) {
        sleep(5);

        for (int i = 0; i < MAX_PIDS; i++) {
            pid_t target_pid = pid_list[i];  // Get the PID of the target process

            if (kill(target_pid, 0) == 0) {
                printf("Process with PID %d is alive.\n", target_pid);
            } else {
                printf("Process with PID %d is not responding.\n", target_pid);

                // Log that the targeted process is not responding
                FILE *log_file = fopen(filename, "a");
                if (log_file != NULL) {
                    fprintf(log_file, "Process with PID %d is not responding.\n", target_pid);
                    fclose(log_file);
                } else {
                    perror("Error opening file for writing");
                }

                // Perform cleanup actions, as in your original code
                for (int j = 0; j < MAX_PIDS; j++) {
                    if (kill(pid_list[j], SIGKILL) == 0) {
                        printf("Sending a SIGKILL to all processes.\n");
                    }
                }

                log_file = fopen(filename, "a");
                if (log_file != NULL) {
                    fprintf(log_file, "ALL PROCESSES ARE TERMINATED.\n");
                    fclose(log_file);
                    brk = true;
                    break;
                } else {
                    perror("Error opening file for writing");
                }
            }
        }
        if (brk)
            break;
    }

    // Close the shared memory object for PIDs (not unlinking yet)
    if (close(shm_fd_pids) == -1) {
        perror("close failed");
        return -1;
    }

    // Unmap the shared memory for PIDs
    if (munmap(pid_list, MAX_PIDS * sizeof(pid_t)) == -1) {
        perror("munmap failed");
        return -1;
    }

    // Unlink the shared memory object for PIDs
    if (shm_unlink(SHM_ID) == -1) {
        perror("shm_unlink failed");
        return -1;
    }

    sem_close(sem_id1);
    sem_close(sem_id2);

    return 0;
}
