#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "core.h"
#include "common.h"

void printFile(struct file_struct *file) {
	unsigned int id = file->id;
	time_t timestamp = (time_t) file->timestamp;
	char *timeString = ctime(&timestamp);
	unsigned int userId = file->userId;

	printf("%u, %u, %u, %s", id, userId, id, timeString);
}

void displayFileList() {
	struct file_list_node *window = fileListHead;

	while (window != NULL) {
		struct file_struct *file = window->file;
		printFile(file);
		window = window->next;
	}
}

void init() {
	loadConfig();
        loadFileList();
}

int main (int argc, char **argv) {
	init();

	displayFileList();

	return 0;	
}
