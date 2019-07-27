#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> //PIPE_BUF
#include <sys/stat.h>
#include <sys/wait.h> //wait()
#include <ctype.h> //isdigit()

ssize_t readln (int fildes, void *buf, size_t nbyte);
int strcopy(char* str, char* str2, int bits);
int split(char *str, char** cmds);
int strToArray(char* buf, char** cmds, int n);
int processCmds(char** cmds,int cmdsLidos);
int processPipedCmds(char **cmds, int cmdsLidos);
int processNumPipedCmds(char** cmds, int cmdsLidos, int cmdAler);
int isEmpty(char* fileName);
