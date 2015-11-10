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

/**
 * Removes the file from the file list and from the file system.
 * This function assumes that the id of the owner of the file
 * and the user have been checked.
 */
void removeFile(struct file_list_node *fileNode, struct file_list_node *prev) {
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

/**
 * Finds the file in the file list, checks that the file belongs to the
 * user running the program and then deletes the file from the list and
 * from the file system.
 */
void removeFileNode(char *nodeId) {
  unsigned long id = atol(nodeId);
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
        printf("%lu: Y\n", id);
      } else {
        printf("%lu: X Can't remove this file, permission denied\n", file->id);
      }

      break;
    }

    prev = window;
    window = window->next;
  }

  if (found == 0) {
    printf("%lu: X File not found\n", id);
    return;
  }
}

/**
 * Goes through all the file ids and removes each one.
 */
void removeFileNodes(int numNodes, char **nodes) {
  int i = 0;
  for (i = 0; i < numNodes; i++) {
    char *node = nodes[i];
    removeFileNode(node);
  }
}

/**
 * Initializes everything needed
 */
void init() {
  initRunners();
  initUmask();
  loadFileList();
}

/**
 * Checks the arguments, calls init, removes the files
 */
int main(int argc, char **argv) {
  init();

  removeFileNodes(argc - 1, argv + 1);

  saveFileList();

  return 0;
}
