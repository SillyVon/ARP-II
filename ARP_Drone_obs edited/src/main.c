#include "./../include/main.h"

int main (int argc, char const *argv[]){

char empty=' ';

// file descriptor for pipes
int input_fd;
int coordinates_fd;
int obstacle_coordinates_fd;
int target_numbers_coordinates_fd;

char *input_fifo = "/tmp/input_fifo";
char *coordinates_fifo = "/tmp/coordinates_fifo";
char *obstacle_coordinates_fifo = "/tmp/obstacle_coordinates";
char *target_numbers_coordinates_fifo = "/tmp/numbers_coordinates";

mkfifo(input_fifo, 0666);
mkfifo(coordinates_fifo, 0666);
mkfifo(obstacle_coordinates_fifo, 0666);
mkfifo(target_numbers_coordinates_fifo, 0666);


//logging

// Open log file for writing
log_file = fopen("/home/yaz/Downloads/ARP_Drone_obs edited/Logs/main_log.txt","w");
if (log_file == NULL) {
    perror("Error opening the file for writing.\n");
    return -1;
}
// Get current time
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);


// Utility variable to avoid trigger resize event on launch
int first_resize = TRUE;

// Initialize User Interface and 
obstacle_coordinates_fd = open(obstacle_coordinates_fifo,O_RDONLY);
ssize_t bytesRead_obstacle = read(obstacle_coordinates_fd, &obstacle, sizeof(obstacle));
close(obstacle_coordinates_fd);

target_numbers_coordinates_fd = open(target_numbers_coordinates_fifo,O_RDONLY);
ssize_t bytesRead_numbers = read(target_numbers_coordinates_fd, &number, sizeof(number));
close(target_numbers_coordinates_fd);

init_console_ui();

nodelay(stdscr, TRUE); // getch() will be non-blocking

// Infinite loop
    while (TRUE) {

    input_fd = open (input_fifo,O_WRONLY);
    int ch = getch(); // non-blocking input
        if (ch !=ERR){
            if (ch == 'Q' || ch == 'q') {
                // If Q is pressed, exit the program
                break;
            }
           
        char direction_input;
        direction_input = (char)ch;
        write(input_fd,&direction_input,sizeof(direction_input));
        }else{
        write(input_fd,&empty,sizeof(empty));
        }
        close (input_fd);

    // i recieve the ee_X ee_y from motion_dynmaics process through pipes 
    coordinates_fd = open(coordinates_fifo,O_RDONLY);
    ssize_t bytesRead;
    //do keeps reading from the pipe until no more data is available. 
    do{
    bytesRead = read(coordinates_fd, &coordinates, sizeof(coordinates));
    if (bytesRead == sizeof(coordinates)){
        update_console_ui(&coordinates.ee_x,&coordinates.ee_y);
    }

    }while ( bytesRead == sizeof(coordinates));
    close (coordinates_fd);
   
    obstacle_coordinates_fd = open(obstacle_coordinates_fifo,O_RDONLY);
    ssize_t bytesRead_obstacle = read(obstacle_coordinates_fd, &obstacle, sizeof(obstacle));
    close(obstacle_coordinates_fd);
    //logging
    fprintf(log_file,"[%s]The User interface is updated.\n Drones coordinates are : (%f, %f)\n",time_string, coordinates.ee_x, coordinates.ee_y);
    fflush(log_file);

    usleep(100000);
}

    // Terminate
    endwin();
    return 0;
}









