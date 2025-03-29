# Cron
This is a project for the Systems Programming course, Summer 2022. It's a cron-like task scheduler written in C.

## Brief Description
- in the [/src](/src) directory, there are two files:  
  - `schedule.c` – contains functions primarily for working with the `schedule.txt` file  
  - `task_scheduler.c` – contains the core code of the scheduler
- in the [/tasks](/tasks) directory, there are files that will be executed  
- the [schedule.txt](schedule.txt) file contains a list of tasks that will be executed at specified intervals
- the [log.txt](log.txt) file records information about the success or failure of task execution
- the program works by waking up every minute and checking whether there are any task that need to be executed
- compiled using:  
  ```sh
  gcc -Wall -Werror -W -o ts task_scheduler.c
  ```
