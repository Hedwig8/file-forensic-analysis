#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>

#ifndef _FORENSIC_H_
#define _FORENSIC_H_

#define READ 0
#define WRITE 1

extern clock_t time0;

void sigint_handler (int signo);

void siguser1_handler (int signo);

void siguser2_handler (int signo);

int isDirectory(const char *name);

void timeConverter(time_t sec, char str[]);

int forkPipeExec(char outputStr[], const char *cmd, const char *filename);

int fileAnalysis(const char *filename);

int forkdir (const char* dirname);

int dirAnalysis(const char* dirname);

void reportLog(const char* report);

#endif