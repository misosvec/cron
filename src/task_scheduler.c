#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h> 
#include "schedule.c"

int check_time(struct schedule schedule){
	time_t t = time(0);
	struct tm* local_time = localtime(&t);
	if(schedule.minute == -1 || schedule.minute == local_time->tm_min){
		if(schedule.hour == -1 || schedule.hour == local_time->tm_hour){
			if(schedule.date == -1 || schedule.date == local_time->tm_mday){
				if(schedule.month == -1 || schedule.month == local_time->tm_mon+1){
					if(schedule.day_of_week == -1 || schedule.day_of_week == local_time->tm_wday){
						return 0;
					}
				}
			}
		}
	}
	return -1;
}

void redirect_stdout_to_file(char filename[]){
	int filefd = open(filename, O_WRONLY | O_CREAT | O_APPEND , 0777);
	if(filefd == -1){
		fprintf(stderr, "failed to open log file (%s)\n", strerror(errno));
		exit(1);
	}

	int dup = dup2(filefd, STDOUT_FILENO);
	if(dup == -1){
		fprintf(stderr, "failed to redirect output to a file (%s)\n", strerror(errno));
		exit(1);
	}
	close(filefd); 
}

void execute_task(struct schedule schedule, char tasks_dirpath[], char log_filepath[]){
	pid_t pid = fork();
	if(pid == -1){
		fprintf(stderr, "fork failed: %s\n", strerror(errno));
	}else if(pid == 0){
		redirect_stdout_to_file(log_filepath);
		char* args[] = {NULL};
		time_t t = time(0);
		struct tm* local_time = localtime(&t);
		char executable[(int)strlen(tasks_dirpath) + (int)strlen(schedule.filename) + 1];
		strcpy(executable, tasks_dirpath);
		strcat(executable, schedule.filename);
		printf("\n[EXECUTING] %s on %s", schedule.filename, asctime(local_time));
		fflush(stdout);
		execvp(executable,args);
        printf("\n[EXECUTION FAILED] %s on %s", schedule.filename, asctime(local_time));
		fflush(stdout);
		exit(1);
	}else{
	    wait(NULL);
	}
}

void check_tasks(struct schedule* schedules, int schedule_count,char tasks_dirpath[], char log_filepath[]){
	int i;
	for(i = 0; i < schedule_count; i++){
		if(check_time(schedules[i]) == 0){
			execute_task(schedules[i], tasks_dirpath, log_filepath);
		}
	}
}

void setup_scheduler(struct schedule* schedules, int schedule_count, char tasks_dirpath[],char log_filepath[]){
	redirect_stdout_to_file(log_filepath);
	
	if(daemon(1, 1) == -1){
	 	fprintf(stderr, "failed to setup scheduler: %s\n", strerror(errno));
		exit(1);
	}

	while(1){
		check_tasks(schedules, schedule_count, tasks_dirpath, log_filepath);
		sleep(60);
	}
}

int main(int argc,char *argv[]){	
	if(argc != 4){
		fprintf(stderr, "invalid arguments, required format is [schedule file path] [tasks directory path] [log file path]");
		exit(1);	
	}
	if (access(argv[1], F_OK) != 0) {
    		fprintf(stderr, "schedule file does not exist\n");
			exit(1);
	}
	if (access(argv[2], F_OK) != 0) {
            fprintf(stderr, "task directory does not exist\n");
			exit(1);
    }
	struct schedule* schedules = malloc(16 * sizeof(struct schedule));
	int schedule_count = read_schedule_file(schedules,argv[1], argv[2]);

	setup_scheduler(schedules, schedule_count, argv[2], argv[3]);
	return 0;	
}