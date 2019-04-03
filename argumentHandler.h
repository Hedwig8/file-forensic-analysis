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

extern bool _r, _h_md5, _h_sha256, _h_sha1, _v;
extern int optind;
extern int log;

void closeFile();

int argumentHandler(int argc, char *argv[]);


#endif

