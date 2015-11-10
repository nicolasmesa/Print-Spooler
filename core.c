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

int ownerId;
int runnerId;
struct config_struct *config;
char *configPath = "/home/user01/spooler/config/config";
char *fileListPath = "/home/user01/spooler/config/file-list";
char *filesPath = "/home/user01/spooler/printer";
struct file_list *fileList;

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

void initUmask() {
  // 0177
  int mask = S_IXUSR | S_IRWXG | S_IRWXO;
  umask(mask);
}

void initRunners() {
  ownerId = geteuid();
  runnerId = getuid();
}

void runAsOwner() { seteuid(ownerId); }

void runAsRunner() { seteuid(runnerId); }

char *getPrintFilePath(unsigned long fileId) {
  int size = strlen(filesPath) + 20;
  char *dst = malloc(size + 1);
  snprintf(dst, size, "%s/%lu", filesPath, fileId);

  return dst;
}

int fdIsValid(int fd) {
  if (fd >= 0 && fd <= 2) {
    return 0;
  }

  return 1;
}

int safeOpen(const char *path, int oflag) {
  int fd = open(path, oflag);

  // This means that one of stdin, stdout or stderr are closed
  if (!fdIsValid(fd)) {
    close(fd);
    exit(1);
  }

  return fd;
}

int safeOpenWithPerms(const char *path, int oflag, int perms) {
  int fd = open(path, oflag, perms);


  // This means that one of stdin, stdout or stderr are closed
  if (!fdIsValid(fd)) {
    close(fd);
    exit(1);
  }

  return fd;
}

void loadDefaultConfig() {
  config = malloc(sizeof(struct config_struct));

  if (config == NULL) {
    printAndExit(NULL);
  }

  config->next_id = 1;
}

void loadConfig() {
  int fd = safeOpen(configPath, O_RDONLY);

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

void initFileList() {
  if (fileList != NULL) {
    return;
  }

  fileList = safeMalloc(sizeof(struct file_list));

  fileList->head = NULL;
  fileList->tail = NULL;
}

void addFileToList(struct file_struct *file) {
  if (fileList == NULL) {
    initFileList();
  }

  struct file_list_node *node = safeMalloc(sizeof(struct file_list_node));

  node->file = file;
  node->next = NULL;

  if (fileList->head == NULL) {
    fileList->head = fileList->tail = node;
  } else {
    fileList->tail->next = node;
    fileList->tail = node;
  }
}

void loadFileList() {
  int fd = safeOpen(fileListPath, O_RDONLY);
  struct file_struct *file;
  int fileStructSize = sizeof(struct file_struct);
  int numRead;

  if (fileList == NULL) {
    initFileList();
  }

  if (fd < 0) {
    if ((int)errno != 2) {
      printAndExit(NULL);
    }

    return;
  }

  file = safeMalloc(sizeof(struct file_struct));

  while ((numRead = read(fd, file, fileStructSize)) == fileStructSize) {
    addFileToList(file);
    file = safeMalloc(sizeof(struct file_struct));
  }

  // The last file allocated is not used.
  free(file);

  close(fd);
}

void saveFileList() {
  if (fileList == NULL) {
    initFileList();
  }

  struct file_list_node *window = fileList->head;
  int fd = safeOpenWithPerms(fileListPath, O_WRONLY | O_CREAT | O_TRUNC, 0600);

  if (fd < 0) {
    printAndExit(NULL);
  }

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
