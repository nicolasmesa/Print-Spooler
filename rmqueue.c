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


void removeFile(struct file_list_node  *fileNode, struct file_list_node *prev) {
	struct file_struct *file = fileNode->file;
	char *filePath = getPrintFilePath(file->id);
	int status = remove(filePath);
	free(filePath);

	if (status < 0) {
		printAndExit(NULL);
	}

	if (prev == NULL) {
		fileList->head = fileNode->next;

		if (fileList->head == NULL) {
			fileList->tail = NULL;
		}

		return;
	}

	prev->next = fileNode->next;
}

void removeFileNode(char *nodeId) {
	unsigned int id = atoi(nodeId);
	struct file_list_node *window = fileList->head;
	struct file_list_node *prev = NULL;
	int found = 0;

	while (window != NULL) {
		struct file_struct *file = window->file;
		if (file->id == id) {
			found = 1;

			if (file->userId == runnerId) {
				removeFile(window, prev);
				free(window);
			} else {
				printf("Can't remove file (%u)\n", file->id);
			}

			break;
		}

		prev = window;
		window = window->next;
	}

	if (found == 0) {
		printf("File %d not found\n", id);
	}
}

void removeFileNodes(int numNodes, char **nodes) {
	// TODO: Make sure running as owner
	int i = 0;
	for (i = 0; i < numNodes; i++) {
		char *node = nodes[i];
		removeFileNode(node);
	}
}

void init() {
	initRunners();
	loadConfig();
        loadFileList();
}

int main (int argc, char **argv) {
	if (argc < 2) {
                printf("Usage: %s fileid1 [fileid2] [fileid3]\n", argv[0]);
                exit(1);
        }

	init();

	removeFileNodes(argc - 1, argv + 1);

	saveFileList();

	return 0;	
}
