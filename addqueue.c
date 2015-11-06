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

void saveConfig() {
	int fd = open(configPath, O_WRONLY | O_CREAT | O_TRUNC, 0600);

	if (fd < 0) {
		printAndExit("Problem opening the config file");
	}

	int ret = write(fd, config, sizeof(struct config_struct));

	if (ret < 0 ) {
		printAndExit(NULL);
	}

	close(fd);
}

void copyFile(int fd, char *dst) {
	char buff[1024];
	int numRead;
	int numWritten;

	// TODO. Make sure you are running as the print spooler
	int dstFd = open(dst, O_CREAT | O_EXCL | O_RDWR, 0600);

	// TODO. The file may already exist.
	if (dstFd < 0 ) {
		printAndExit(NULL);
	}

	while ((numRead = read(fd, buff, 1023)) == 1023) {
		numWritten = write(dstFd, buff, numRead);

		if (numWritten < 0 ) {
			printAndExit(NULL);
		}

		if (numWritten != numRead) {
			printAndExit("Num written different than num read");
		}
	}

	numWritten = write(dstFd, buff, numRead);

        if (numWritten != numRead) {
                printAndExit("Num written different than num read");
        }

	close(dstFd);
}

void addFileToQueue(char *filePath) {
	int fileId = config->next_id;
	struct file_struct *file;

	if (filePath == NULL) {
		return;
	}


	runAsRunner();
	int fd = open(filePath, O_RDONLY);
	runAsOwner();

	if (fd < 0) {
		printf("\tError: %s\n", strerror(errno));
	}

	char *dst = getPrintFilePath(fileId);

	copyFile(fd, dst);

	free(dst);

	close(fd);

	file = safeMalloc(sizeof(struct file_struct));

	file->id = fileId;
	file->timestamp = (int) time(NULL);
	file->userId = runnerId;

	addFileToList(file);
	config->next_id++;
}

void addFilesToQueue(int numFiles, char **filePaths) {
        int i = 0;

        for (i = 0; i < numFiles; i++) {
		addFileToQueue(filePaths[i]);
        }
}

void init() {
	initRunners();

	int mask = S_IXUSR | S_IRWXG | S_IRWXO;
	umask(mask);
	loadConfig();
	loadFileList();
}

void end() {
	saveConfig();
	saveFileList();
}


int main (int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: %s filename1 [filename2] [filename3]\n", argv[0]);
		exit(1);
	}

	init();

	addFilesToQueue(argc - 1, argv + 1);
	
	end();
	return 0;
}
