#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>

// defining the shared memory and semaphores
#define SHM_ID "/pids_shared_memory"
#define SEM_PATH_1 "/pid_path_1"
#define SEM_PATH_2 "/pid_path_2"

#define MAX_PIDS 5 // to be modified according to the number of processes needed.

//logging

// initializing log file
FILE *log_file;
time_t current_time;
struct tm *time_info;
char time_string[20];


int main (int argc, char *argv[]){

//pids of the children processes
pid_t child_pid;
pid_t child_main;
pid_t child_obstacle;
pid_t child_motion;
pid_t child_target_numbers;
pid_t child_watchdog;
// Array to store child process IDs
pid_t child_pids[MAX_PIDS] = {child_main, child_motion, child_obstacle, child_target_numbers ,child_watchdog};
pid_t *pid_list;

int res;

//logging

// Open log file for writing
log_file = fopen("/home/yaz/Downloads/ARP_Drone_obs edited/Logs/server_log.txt","w");
if (log_file == NULL) {
    perror("Error opening the file for writing.\n");
    return -1;
}
// Get current time
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

//create a shared memory object for the PIDs
int shm_fd_pid = shm_open (SHM_ID,O_CREAT|O_RDWR,0666);
if (shm_fd_pid==-1){
  perror("opening share memory failed");
  return -1;
} 

//mapping the memory
pid_list=(pid_t*)mmap(NULL, MAX_PIDS *sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_pid,0);
if (pid_list== MAP_FAILED){
  perror("mapping failed");
  exit(EXIT_FAILURE);
}

//create semaphores

sem_t *sem_id1 = sem_open (SEM_PATH_1,O_CREAT,0666,1);
sem_t *sem_id2 = sem_open (SEM_PATH_2,O_CREAT,0666,0);

// init semaphores 
sem_init(sem_id1,1,1);
sem_init(sem_id2,1,0);


//Make main child process
fprintf(log_file,"[%s]Forking a Main process\n",time_string);
fflush(log_file);
child_main = fork();
if(child_main < 0){
    perror("error while forking...");
    return -1;
}

if (child_main == 0){
    char* arg_list_main[]= {"/usr/bin/konsole", "-e", "/home/yaz/Downloads/ARP_Drone_obs edited/bin/main", NULL };
    execvp("/usr/bin/konsole", arg_list_main);
    return 0;
}

//Make motion child process
fprintf(log_file,"[%s]Forking the Motion_Dynamics process\n",time_string);
fflush(log_file);
child_motion = fork();
if (child_motion < 0){
    perror("error while forking...");
    return -1;
}
if (child_motion == 0){
    char* arg_list_motion[]= {"/usr/bin/konsole", "-e","/home/yaz/Downloads/ARP_Drone_obs edited/bin/motion", NULL };
    execvp("/usr/bin/konsole", arg_list_motion);
    return 0;
}

//Make obstacel child process 
fprintf(log_file,"[%s]Forking the Obstacle_Generator process\n",time_string);
fflush(log_file);
child_obstacle = fork();
if (child_obstacle < 0){
    perror("error while forking ...");
    return -1;
}
if ( child_obstacle == 0){
    char* arg_list_obstacle[]= {"/usr/bin/konsole", "-e","/home/yaz/Downloads/ARP_Drone_obs edited/bin/obstacles", NULL };
    execvp("/usr/bin/konsole", arg_list_obstacle);
    return 0;
}

//Make target_numbers child process
fprintf(log_file,"[%s]Forking the Numbers_Generator process\n",time_string);
fflush(log_file);
child_target_numbers = fork();
if (child_target_numbers < 0){
    perror("error while forking ...");
    return -1;
}
if ( child_target_numbers == 0){
    char* arg_list_target_numbers[]= {"/usr/bin/konsole", "-e","/home/yaz/Downloads/ARP_Drone_obs edited/bin/target_numbers", NULL };
    execvp("/usr/bin/konsole", arg_list_target_numbers);
    return 0;
}

//Make watchdog child process
fprintf(log_file,"[%s]Forking the Watchdog process\n",time_string);
fflush(log_file);
child_watchdog = fork();
if (child_watchdog < 0){
    perror("error while forking ...");
    return -1;
}
if ( child_watchdog == 0){
    char* arg_list_watchdog[]= {"/usr/bin/konsole", "-e","/home/yaz/Downloads/ARP_Drone_obs edited/bin/watchdog", NULL };
    execvp("/usr/bin/konsole", arg_list_watchdog);
    return 0;
}

fprintf(log_file,"[%s]Process Main forked successfully,Process ID: %d\n",time_string, child_main);
fflush(log_file);
fprintf(log_file,"[%s]Process Motion_dynamics forked successfully,Process ID: %d\n",time_string, child_motion);
fflush(log_file);
fprintf(log_file,"[%s]Process Obstacle_Generator forked successfully,Process ID: %d\n",time_string, child_obstacle);
fflush(log_file);
fprintf(log_file,"[%s]Process Numbers_Generator forked successfully,Process ID: %d\n",time_string, child_target_numbers);
fflush(log_file);
fprintf(log_file,"[%s]Process Watchdog forked successfully,Process ID: %d\n",time_string, child_watchdog);   
fflush(log_file);

//store the pids in the shared memory

int t = sem_wait(sem_id1); // wait reader
if(t!=0){
  perror("sem_wait failed ");
  fprintf(log_file,"[%s]sem_wait failed\n",time_string);
  exit(EXIT_FAILURE);
 }
fprintf(log_file,"[%s]semaphore wait return value\n",time_string);
fflush(log_file);
// Copy the child process IDs to the shared memory
for (int i = 0; i < MAX_PIDS; i++) {
    pid_list[i] = child_pids[i];
    fprintf(log_file, "[%s]Process ID %d written to shared memory, PID: %d\n", time_string, i, pid_list[i]);
    fflush(log_file);
}
sem_post(sem_id2); // restart reader 

// wait for children to finish

for(int i = 0; i < MAX_PIDS; i++) {
    child_pid = wait(&res);
    printf("Child process with PID %d exited with status %d\n", child_pid, res);
}

    munmap(pid_list, MAX_PIDS * sizeof(pid_t));
    shm_unlink(SHM_ID);
    sem_unlink(SEM_PATH_1);
    sem_unlink(SEM_PATH_2);

    // Close log file
    fclose(log_file);

    return 0;
}