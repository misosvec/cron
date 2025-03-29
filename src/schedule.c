#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 

struct schedule{
	int minute;
	int hour;
	int date;
	int month;
	int day_of_week;
	char* filename;
};

struct schedule is_schedule_valid(char line[], char task_dir_filepath[]){
	struct schedule s;
	
	char dummy;
	
	if (sscanf(line, "%d %d %d %d %d %ms %s", &s.minute, &s.hour, &s.date, &s.month, &s.day_of_week, &s.filename, &dummy) != 6){
		fprintf(stderr, "invalid schedule: %s\n", line);
		exit(1);
	}
	if (s.minute != -1 && (s.minute < 0 || s.minute > 59)){
		fprintf(stderr, "invalid minute: %s\n", line);
		exit(1);
	}
	if(s.hour != -1 && (s.hour < 0 || s.hour > 23)){
		fprintf(stderr, "invalid hour: %s\n", line);
		exit(1);
	}
	if(s.date != -1 && (s.date < 1 || s.date > 31)){
		fprintf(stderr, "invalid date: %s\n", line);
		exit(1);
	}
	if(s.month != -1 && (s.month < 1 || s.month > 12)){
		fprintf(stderr, "invalid month: %s\n", line);
		exit(1);
	}
	if(s.day_of_week != -1 && (s.day_of_week < 0 || s.day_of_week > 6)){
		fprintf(stderr, "invalid day of week: %s\n", line);
		exit(1);
	}
	char task[(int)strlen(task_dir_filepath) + (int)strlen(s.filename) + 1];
	strcpy(task, task_dir_filepath);
	strcat(task, s.filename);
	if (access(task, F_OK) != 0) {
                fprintf(stderr,"task does not exist: %s\n", line);
                exit(1);
        }
	return s;
}

int read_schedule_file(struct schedule *schedules, char schedule_filepath[], char task_dir_filepath[]){
	int schedule_count = 0;
	int schedules_size = 16;
	int fdin = open(schedule_filepath, O_RDONLY);
	if (fdin == -1){
		fprintf(stderr, "file: %s\n%s\n", schedule_filepath, strerror(errno));
		exit(1);
	}
	char buffer;
	int bytes;
	int line_size = 32;
	char *line = malloc(line_size);
	int len = 0;
	while ((bytes = read(fdin, &buffer, 1) > 0)){
		if(len == line_size){
			line_size += line_size;
			line = realloc(line, line_size);
			if(line == NULL){
				fprintf(stderr, "Out of memory!\n");
			}
		}
		if (buffer == '*'){
			line[len] = '-';
			line[len + 1] = '1';
			len += 2;
		}else if (buffer == '\n' || buffer == '\0'){
			line[len] = '\0';
			len = 0;
			if (schedule_count == schedules_size){
				schedules_size += schedules_size;
				schedules = realloc(schedules, schedules_size * sizeof(struct schedule));
				if(schedules == NULL){
					fprintf(stderr, "Out of memory!\n");
					exit(1);
				}
			}
			schedules[schedule_count] = is_schedule_valid(line, task_dir_filepath);
			schedule_count++;
		}else{
			line[len] = buffer;
			len++;
		}
	}
	free(line);
	if (close(fdin)){
		fprintf(stderr, "file: %s\n%s\n", schedule_filepath, strerror(errno));
		exit(1);
	}
	return schedule_count;
}