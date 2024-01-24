#include <ncurses.h>
#include <string.h>
#include <unistd.h> 
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <stdio.h>  
#include <sys/stat.h>
#include <unistd.h>

#define MAX_NUMBERS 9

FILE *log_file;
time_t current_time;
struct tm *time_info;
char time_string[20];

// limits for the field dimenstions. // this value could be recieved from the main for adjustment 
int DRONE_X_LIM = 100;
int DRONE_Y_LIM = 20;

typedef struct {
    int x;
    int y;
    int value;
    
} Number;

Number number[MAX_NUMBERS];

//Drone coordinates 
typedef struct {
    float ee_y; // the current position 
    float ee_x;  // the current position 
}Coordinates;

Coordinates coordinates;

//Functions

// Utility methods to spawn random container within the hoist's workspace
void initialize_numbers() {
    srand(time(NULL));

    for (int i = 0; i < MAX_NUMBERS; i++) {
        number[i].value = i + 1;  // Numbers from 1 to 9
        number[i].x = rand() % DRONE_X_LIM;
        number[i].y = rand() % DRONE_Y_LIM;
    }
}

int main (int argc, char const *argv[]){

int numbers_coordinates_fd;
char *numbers_coordinates_fifo = "/tmp/numbers_coordinates";
mkfifo(numbers_coordinates_fifo, 0666);

//logging

    // Open log file for writing
    log_file = fopen("/home/yaz/Downloads/ARP_Drone_obs edited/Logs/Numbers_log.txt","w");
    if (log_file == NULL) {
        perror("Error opening the file for writing.\n");
        return -1;
    }
    // Get current time
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);


initialize_numbers ();
numbers_coordinates_fd = open(numbers_coordinates_fifo,O_WRONLY);
write(numbers_coordinates_fd,&number,sizeof(number));
close (numbers_coordinates_fd);

for (int i = 0; i < MAX_NUMBERS; i++) {
    fprintf(log_file,"[%s] Number : %d coordinates : x = %d , y = %d\n",time_string,i+1,number[i].x,number[i].y);
    fflush(log_file);
    }

return 0;

}

