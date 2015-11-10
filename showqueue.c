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
 * Prints the information of the file to stdout
 */
void printFile(struct file_struct *file) {
  unsigned long id = file->id;
  time_t timestamp = (time_t)file->timestamp;
  struct tm *tminfo = localtime(&timestamp);
  char timeString[100];
  unsigned int userId = file->userId;

  strftime(timeString, 100, "%F_%H:%M", tminfo);

  printf("%lu %u %s %lu\n", id, userId, timeString, id);
}

/**
 * Prints the file list to stdout
 */
void displayFileList() {
  struct file_list_node *window = fileList->head;

  while (window != NULL) {
    struct file_struct *file = window->file;
    printFile(file);
    window = window->next;
  }
}

/**
 * Initializes everyting needed by this program
 */
void init() {
  initUmask();
  loadFileList();
}

/**
 * Main function. Initializes and the displays the file list
 */
int main(int argc, char **argv) {
  init();

  displayFileList();

  return 0;
}
