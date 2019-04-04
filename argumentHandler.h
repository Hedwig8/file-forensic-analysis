#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

#ifndef _ARGUMENTHANDLER_H_
#define _ARGUMENTHANDLER_H_

extern bool _r, _h_md5, _h_sha256, _h_sha1, _v, _o;
extern int optind;
extern int logFd;
extern char* outputFile;

void closeFile();

void closeLogFile();

int argumentHandler(int argc, char *argv[]);


#endif

