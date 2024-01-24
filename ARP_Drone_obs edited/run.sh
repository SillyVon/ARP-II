#!/bin/bash

# Compile server
gcc src/server_.c -o bin/server_new -lm -lncurses

# Compile main
gcc src/main.c -o bin/main -lm -lncurses

# Compile motion_dynamics
gcc src/motion_dynamics.c -o bin/motion -lm -lncurses

# Compile obstacles_spawner
gcc src/obstacles_spawner.c -o bin/obstacles -lm -lncurses

# Compile target_numbers
gcc src/target_numbers.c -o bin/target_numbers -lm -lncurses

# Compile watchdog
gcc src/watchdog.c -o bin/watchdog -lm -lncurses

# Run server_new
./bin/server_new

