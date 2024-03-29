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
 * Saves the config file, overwritting the original file if it existed
 * already.
 */
void saveConfig() {
  int fd = safeOpenWithPerms(configPath, O_WRONLY | O_CREAT | O_TRUNC, 0600);

  if (fd < 0) {
    printAndExit("Problem opening the config file");
  }

  int ret = write(fd, config, sizeof(struct config_struct));

  if (ret < 0) {
    printAndExit(NULL);
  }

  close(fd);
}

/**
 * Copies the file descriptor to the destination passed as the argument.
 */
void copyFile(int fd, char *dst) {
  char buff[1024];
  int numRead;
  int numWritten;

  int dstFd = safeOpenWithPerms(dst, O_CREAT | O_EXCL | O_RDWR, 0600);

  // The file may already exist. Following advice in
  // http://www.homeport.org/~adam/setuid.7.html
  // We terminate the program in response to a strange situation rather than
  // attepting to
  // carry on. One solution here could be incrementing config->file_id and
  // calling this
  // function again
  if (dstFd < 0) {
    printAndExit(NULL);
  }

  while ((numRead = read(fd, buff, 1023)) == 1023) {
    numWritten = write(dstFd, buff, numRead);

    if (numWritten < 0) {
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

/**
 * Tries to open a file while running as the user running the command
 * and, if it is successful, the file is copied to the printer
 * directory and added to the file list
 */
void addFileToQueue(char *filePath) {
  unsigned long fileId = config->next_id;
  struct file_struct *file;
  struct stat statBuf;

  runAsRunner();
  int fd = safeOpen(filePath, O_RDONLY);
  runAsOwner();

  if (fd < 0) {
    printf("%s: X %s\n", filePath, strerror(errno));
    return;
  }

  // Make sure only regular files are sent to print
  int statRet = fstat(fd, &statBuf);

  if (statRet) {
    printf("%s: X %s\n", filePath, strerror(errno));
    close(fd);
    return;
  }

  if (!S_ISREG(statBuf.st_mode)) {
    printf("%s: X Only regular files are allowed\n", filePath);
    close(fd);
    return;
  }

  config->next_id++;
  saveConfig();

  char *dst = getPrintFilePath(fileId);

  copyFile(fd, dst);

  free(dst);

  close(fd);

  file = safeMalloc(sizeof(struct file_struct));

  file->id = fileId;
  file->timestamp = (int)time(NULL);
  file->userId = runnerId;

  addFileToList(file);

  printf("%s: Y %lu\n", filePath, fileId);
}

/**
 * Iterates through the files passed and adds them to
 * the file queue.
 */
void addFilesToQueue(int numFiles, char **filePaths) {
  int i = 0;

  for (i = 0; i < numFiles; i++) {
    addFileToQueue(filePaths[i]);
  }
}

/**
 * Initializes everything needed to run the addqueue command
 */
void init() {
  initRunners();

  initUmask();
  loadConfig();
  loadFileList();
}

/**
 * Saves the new config file and the file list
 */
void end() {
  saveConfig();
  saveFileList();
}

/**
 * Main function. Checks the arguments, calls init,
 * processes the files and calls end
 */
int main(int argc, char **argv) {
  init();

  addFilesToQueue(argc - 1, argv + 1);

  end();
  return 0;
}
