#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


struct config_struct {
	unsigned int next_id;
};

struct file_struct {
	unsigned int id;
	unsigned int timestamp;
	unsigned int userId;
};

struct file_list_node {
	struct file_list_node *next;
	struct file_struct *file;
};

static struct config_struct *config;
static char *configPath = "config/config";
static char *filesPath = "printer";
static struct file_list_node *fileListHead;

static int ownerId;
static int runnerId;

void printAndExit(char *msg) {
	if (msg == NULL) {
		printf("Error: %s\n", strerror(errno));
		exit(errno);
	}

	printf("Error: %s\n", msg);
	exit(1);
}

void runAsOwner() {
	seteuid(ownerId);
}

void runAsRunner() {
	seteuid(runnerId);
}

void loadDefaultConfig() {
	config = malloc(sizeof(struct config_struct));

	if (config == NULL) {
		printAndExit(NULL);
	}

	config->next_id = 1;
}

void loadConfig() {
	int fd = open(configPath, O_RDONLY);

	if (fd < 0) {
		loadDefaultConfig();
		return;
	}

	config = malloc(sizeof(struct config_struct));

	if (config == NULL) {
		printAndExit(NULL);
	}


	int ret = read(fd, config, sizeof(struct config_struct));

	if (ret < 0) {
		printAndExit(NULL);
	}

	close(fd);
}

void saveConfig() {
	int fd = open(configPath, O_WRONLY | O_CREAT | O_TRUNC, 0777);

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

		printf("numwritten (%d), numread(%d)\n", numWritten, numRead);

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
	if (filePath == NULL) {
		return;
	}

	int fileId = config->next_id;

	runAsRunner();
	int fd = open(filePath, O_RDONLY);
	runAsOwner();

	if (fd < 0) {
		printf("\tError: %s\n", strerror(errno));
	}

	char *dst = malloc(strlen(filesPath) + 21);

	sprintf(dst, "%s/%d", filesPath, fileId);

	printf("dst: (%s)\n", dst);

	copyFile(fd, dst);

	free(dst);

	close(fd);

	config->next_id++;
}

void addFilesToQueue(int numFiles, char **filePaths) {
        int i = 0;

        for (i = 0; i < numFiles; i++) {
		addFileToQueue(filePaths[i]);
        }
}

void init() {
	ownerId = geteuid();
	runnerId = getuid();

	int mask = S_IXUSR | S_IRWXG | S_IRWXO;
	umask(mask);
	loadConfig();
}

void end() {
	saveConfig();
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
