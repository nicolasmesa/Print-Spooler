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
  unsigned long next_id;
};

struct file_struct {
  unsigned long id;
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

/**
 * Prints error to screen and exits the program.
 * This function should be called as a last resort.
 */
void printAndExit(char *msg);

/**
 * Calls malloc and makes sure that something is
 * returned. If not, it exits. (Does the if (ptr == NULL)
 * check for the caller.
 */
void *safeMalloc(size_t len);

/**
 * Duplicates memory and returns a new pointer poiting to
 * the newly allocated and duplicated piece of memory
 */
void *dupMem(void *src, size_t len);

/**
 * Sets the effective user to the owner of the file. Note that
 * the executable needs to be setuid
 */
void runAsOwner();

/**
 * Sets the effective user to the user who is running the program
 */
void runAsRunner();

/**
 * Creates a string containing the path to the file with
 * some file id.
 */
char *getPrintFilePath(unsigned long fileId);

/**
 * Opens the file and returns the file descriptor only
 * if the file descriptor is not one of stdin, stdout,
 * stderr. Note that it doesn't check for negative values,
 * this check is left to the caller.
 */
int safeOpen(const char *path, int oflag);

/**
 * Opens the file and returns the file descriptor only
 * if the file descriptor is not one of stdin, stdout,
 * stderr. Note that it doesn't check for negative values,
 * this check is left to the caller.
 */
int safeOpenWithPerms(const char *path, int oflag, int perms);

/**
 * Sets the variables of the runner id and the owner id
 */
void initRunners();

/**
 * Initializes the umask
 */
void initUmask();

/**
 * Called when the config file doesn't exists. Populates the
 * config struct with the default configuration
 */
void loadDefaultConfig();

/**
 * Loads the configuration from the config file
 */
void loadConfig();

/**
 * Creates a new list node in the file list and adds
 * the file to the end of the list (tail)
 */
void addFileToList(struct file_struct *file);

/**
 * Loads the files from the files file and adds them to
 * the file list
 */
void loadFileList();

/**
 * Saves the file list to the files file
 */
void saveFileList();

#endif
