#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "argumentHandler.h"


extern int dirNumber, fileNumber;
extern struct timespec time0;


int isDirectory(const char *name);

void execTimeConverter(char str[]);

void realTimeConverter(time_t sec, char str[]);

void finalMessages();

#endif