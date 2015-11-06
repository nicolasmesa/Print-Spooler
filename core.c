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

struct config_struct *config;
char *configPath = "config/config";
char *fileListPath = "config/file-list";
char *filesPath = "printer";
struct file_list_node *fileListHead;

void printAndExit(char *msg) {
        if (msg == NULL) {
                printf("Error: %s\n", strerror(errno));
                exit(errno);
        }

        printf("Error: %s\n", msg);
        exit(1);
}

void *safeMalloc(size_t len) {
        void *new = malloc(len);

        if (new == NULL) {
                printAndExit(NULL);
        }

        return new;
}

void *dupMem(void *src, size_t len) {
        void *dst = safeMalloc(len);
        memcpy(dst, src, len);
        return dst;
}

void initRunners() {
	ownerId = geteuid();
        runnerId = getuid();
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

        printf("Loaded config. Next id (%d)\n", config->next_id);

        close(fd);
}

void addFileToList(struct file_struct *file) {
        struct file_list_node *node = safeMalloc(sizeof(struct file_list_node));

        node->file = file;
        node->next = fileListHead;
        fileListHead = node;
}


void loadFileList() {
        int fd = open(fileListPath, O_RDONLY);
        struct file_struct *file;
        int fileStructSize = sizeof(struct file_struct);
        int numRead;


        if (fd < 0) {
                if ((int) errno != 2) {
                        printAndExit(NULL);
                }

                fileListHead = NULL;
                return;
        }

        file = safeMalloc(sizeof(struct file_struct));

        while ((numRead = read(fd, file, fileStructSize))== fileStructSize) {
                addFileToList(file);
                file = safeMalloc(sizeof(struct file_struct));
        }

        // The last file allocated is not used.
        free(file);

        close(fd);
}

void saveFileList() {
        struct file_list_node *window = fileListHead;
        int fd = open(fileListPath, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        //@TODO make sure we are running as owner

        if (fd < 0) {
                printAndExit(NULL);
        }

        printf("Called\n");

        while (window != NULL) {
                struct file_struct *file = window->file;
                int numWritten = write(fd, file, sizeof(struct file_struct));

                if (numWritten != sizeof(struct file_struct)) {
                        printAndExit("numWritten different than size of file struct");
                }

                window = window->next;
        }

        close(fd);
}
