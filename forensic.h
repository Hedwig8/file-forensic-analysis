#ifndef _FORENSIC_H_
#define _FORENSIC_H_

#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include "argumentHandler.h"
#include "utils.h"


#define READ 0
#define WRITE 1


void sigint_handler (int signo);

void siguser1_handler (int signo);

void siguser2_handler (int signo);

int forkPipeExec(char outputStr[], const char *cmd, const char *filename);

int fileAnalysis(const char *filename);

int forkdir (const char* dirname);

void reportLog(const char* report);

int dirAnalysis(const char* dirname);


#endif
