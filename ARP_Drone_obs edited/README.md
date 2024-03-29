
# ARP ASSIGNMENT I
The Drone Operation Interactive Simulator is a C-based software application designed to provide a realistic simulation of drone operation in a 2-degree-of-freedom environment. Users can control the drone using keyboard inputs, experiencing dynamic movements with mass and viscous resistance.



# Dependencies

This project utilizes the following libraries:

### Standard C Libraries

- `stdio.h`
- `stdlib.h`
- `sys/types.h`
- `unistd.h`
- `sys/wait.h`
- ...

### POSIX Libraries

- `sys/mman.h`
- `fcntl.h`
- `semaphore.h`
- `time.h`
- ...

### Third-Party Libraries

-`ncurses.h`: Used for handling terminal input and output in a more advanced manner.

Please ensure that you have the necessary versions of these libraries installed on your system before attempting to compile and run the project.
# Key Features

Controls:

Use the following keys for navigation:
- W: Move North
- E: Move Northeast
- R: Move Northwest
- S: Move South
- D: Move East
- F: Move West
- X: Move Southwest
- C: Move Southeast

Additional controls:
- Q: Quit the program
- Z: Reset the drone
- D: Stop the drone

Drone Control: Navigate the drone in 8 directions, adjust velocity, stop, reset, suspend, and quit the simulation.

Realistic Dynamics: Modelled as a 2-degree-of-freedom dot with mass and viscous resistance for authentic movement.

Software Architecture: Includes a server managing a blackboard with the world map's geometrical state, facilitating interaction with targets and obstacles.

Logging: Values can be logged for further analysis.

Watchdog Process: Monitors system activity, sending notifications and stopping the system if no computation is detected within a configurable time.
# Architecture 
This architecture involves six active components, each running in a separate process.

## Server Process :

The server process serves as the main controlling entity that initiates and manages the various components of the drone system.

### Forking Processes :

The server process initiates the creation of multiple child processes using the fork system call. Each child process represents a different component of the drone system.

### Child Processes :

This architecture involves six active components, each running in a separate process:

### Main Process:
- Responsible for initializing the user interface.
- Manages a blackboard with the geometrical state of the world,          including the map, drone, targets, and obstacles.
- Handles the communication between different processes.
- Monitors and manages the overall execution flow.

#### Functionality :

Initialization:
The init_console_ui function initializes the ncurses-based console user interface, including creating buttons and initializing the drone's field.

User Input and Direction Tracking:
The program listens for user keyboard input in a non-blocking mode (getch()). then updates the Motion and Dynamics process with the input via an initialized pipe, which later will return the updated coordinates of the drone.

### Motion and Dynamics Process :
- Receives user input from the keyboard.
- Uses internal functions to determine the direction, forces, and creates the motion of the drone.
- Constantly updates the drone's coordinates based on the dynamics of the system.
- Calculates repulsive forces generated by obstacles using a function basied on Latombe/Kathib's model.
#### Functionality:

The direction_tracker function interprets the user input and determines the motion direction for the drone.

Drone Motion Control:
The program uses a switch-case structure to control the drone's motion based on the specified direction.
Forces on the X and Y axes are updated according to the selected motion direction.

#### Algorithms:

- Motion Method:
     drone in the update_console_ui function.

- Repulsive forces 
    The drone incorporates a collision avoidance algorithm using repulsive forces. This algorithm, found in the `repulsive_forces` function in `Motion_Dynamics.c`, ensures the drone reacts to obstacles in its environment.






### Target Numbers Generation Process:
- Generates targets for the drone to navigate towards.
- Communicates with the Main Process to update the blackboard with target information.

### Obstacle Generation Process:
- Generates obstacles in the drone's environment.
- Communicates with the Main Process to update the blackboard with obstacle information.
- Communicates with the Montion and Dynamics process to update it with the obstacle coordinates and calculate the repulsive forces.

### Watchdog Process:
- Monitors the system for any signs of inactivity or issues.
- Sends notifications if no computation is going on (timeout condition) or based on predefined criteria.
- Initiates the shutdown of the entire system if necessary.

#### Functionality:

Shared Memory and Semaphores Initialization:
Opens shared memory (SHM_ID) and semaphores (SEM_PATH_1 and SEM_PATH_2) created by the server.

- Waiting for Master to Write:
    Uses sem_wait on sem_id_2 to wait for the server to write data to the shared memory.

- Reading PIDs from Shared Memory:
    Maps the shared memory into the address space for PIDs using mmap.
    Displays the received PIDs and restarts the server process by signaling (SIGUSR1) it.

- Watchdog Loop:
    Periodically checks the status of the target processes (PIDs received from the server).
    Sends a SIGUSR1 signal to each target process to check if it is responsive.

- Logging:
    Logs the status of the target processes, successful signals, and any unresponsive processes to a log file (Logs/watchdog_log.txt).
    If a process is not responding, it sends a SIGKILL signal to terminate all processes and logs the event.

- Cleanup:
    Unmaps the shared memory for PIDs and unlinks the shared memory object.
    Closes semaphores (sem_id1 and sem_id2).


### Communication Process:
- Manages communication between different components using pipes.
- Handles the flow of information between the Main Process, Motion Dynamics Process, Target Generation Process, and Obstacle Generation Process.
- shared memory is used to secure the communication between the server and the watchdog process.

### Logfiles:
- Logfiles capture the values and events during execution.


 # Getting Started 

To compile and run the project, open a terminal and execute the following command:
```console
bash run.sh
```
This script compiles the source files (server.c, main.c, and watchdog.c) using GCC with the necessary flags and then executes the compiled server binary.

Using the Makefile

Alternatively, you can use the Makefile located in the root directory of the project. Open a terminal and run the following commands:
```console
make
```
This will compile all source files and generate the corresponding binaries in the bin directory. After compilation, you can run the server using:
```console
./bin/server
```
# Installation

Ensure that you have the required dependencies installed on your system (details in the Dependencies section).

### Clone the repository
- `https://github.com/SillyVon/ARP-II.git`

### Navigate to the project directory
cd your-repository

### Run the installation script to compile source files and start the server

- `./run.sh` 

## Quick Architecture Sketch

