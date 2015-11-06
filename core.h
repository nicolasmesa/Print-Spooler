#ifndef CORE
#define CORE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

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

struct file_list {
	struct file_list_node *head;
	struct file_list_node *tail;
};

void printAndExit(char *msg);

void *safeMalloc(size_t len);

void *dupMem(void *src, size_t len);

void runAsOwner();

void runAsRunner();

void initRunners();

void loadDefaultConfig();

void loadConfig();

void addFileToList(struct file_struct *file);

void loadFileList();

void saveFileList();

#endif
