#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define TRUE 1
#define MAX_OBSTACLES 10


int DRONE_X_LIM = 100;
int DRONE_Y_LIM = 20;

FILE *log_file;
time_t current_time_log;
struct tm *time_info;
char time_string[20];

typedef struct {
    int x;
    int y;
} Obstacle;

Obstacle obstacle[MAX_OBSTACLES];

void spawn_random_obstacle() {
    srand(time(NULL));
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacle[i].x = rand() % DRONE_X_LIM;
        obstacle[i].y = rand() % DRONE_Y_LIM;
    }
}

int main(int argc, char const *argv[]) {

    int obstacle_coordinates_fd;
    int obstacle_motion_fd;

    char *obstacle_coordinates_fifo = "/tmp/obstacle_coordinates";
    char *obstacle_motion_coordinates_fifo = "/tmp/obstacle_motion";

    mkfifo(obstacle_coordinates_fifo, 0666);
    mkfifo(obstacle_motion_coordinates_fifo,0666);

    //logging

    // Open log file for writing
    log_file = fopen("/home/yaz/Downloads/ARP_Drone_obs edited/Logs/Obstacles_log.txt","w");
    if (log_file == NULL) {
        perror("Error opening the file for writing.\n");
        return -1;
    }
    // Get current time
        time(&current_time_log);
        time_info = localtime(&current_time_log);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);


    clock_t start_time, current_time;
    double elapsed_time;

    // Set the initial start time
    start_time = clock();

    // Define the maximum number of iterations 
    int max_iterations = 10000;
    int iteration_count = 0;

    spawn_random_obstacle();

    obstacle_coordinates_fd = open(obstacle_coordinates_fifo, O_WRONLY|O_NONBLOCK);
    write(obstacle_coordinates_fd, &obstacle, sizeof(obstacle));
    close(obstacle_coordinates_fd);

    while (TRUE) {

        // Check if enough time has passed since the last obstacle generation
        
        current_time = clock();
        elapsed_time = ((double)(current_time - start_time)) / CLOCKS_PER_SEC;

        if (elapsed_time > 5.0) { 
            spawn_random_obstacle();
            
            obstacle_motion_fd = open (obstacle_motion_coordinates_fifo,O_WRONLY| O_NONBLOCK);
            write(obstacle_motion_fd,&obstacle, sizeof(obstacle));
            close(obstacle_motion_fd);
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                fprintf(log_file,"[%s] Obstacle %d Generation update : (%d, %d)\n",time_string, i, obstacle[0].x, obstacle[0].y);
                fflush(log_file);
            }
        
            
            start_time = clock();  // Reset the start time for the next obstacle generation
        }

        obstacle_coordinates_fd = open(obstacle_coordinates_fifo, O_WRONLY|O_NONBLOCK);
        write(obstacle_coordinates_fd, &obstacle, sizeof(obstacle));
        close(obstacle_coordinates_fd);
            
            //debug lines
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                printf("Obstacle: (%d, %d)\n", obstacle[i].x, obstacle[i].y);
            }

        
    
        }
       
    return 0;
        
    }

  

