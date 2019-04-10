#include "utils.h"

struct timespec time0;
int fileNumber = 0;
int dirNumber = 0;

int isDirectory(const char *name)
{
    struct stat dir;
    if (lstat(name, &dir))
    {
        return 0;
    }
    return S_ISDIR(dir.st_mode);
}

void execTimeConverter(char str[])
{
    struct timespec time1;
    clock_gettime(CLOCK_REALTIME, &time1);

    long msec = (time1.tv_nsec - time0.tv_nsec) / 1000000;
    long sec;
    if (msec < 0)
    {
        sec = time1.tv_sec - time0.tv_sec - 1;
        msec += 1000;
        msec %= 1000;
    }
    else
        sec = time1.tv_sec - time0.tv_sec;

    sprintf(str, "%ld.%03ld", sec, msec);
}

void realTimeConverter(time_t sec, char str[])
{
    struct tm time;
    localtime_r(&sec, &time);
    strftime(str, sizeof("dd-mm-yyyyThh:mm:ss"), "%FT%H:%M:%S", &time);
}


void finalMessages() {

    char dirFileNumMessage[73];
    sprintf(dirFileNumMessage, "\r\nExecution finished and a total of %d/%d directories/files were analized", dirNumber, fileNumber);
    write(STDERR_FILENO, dirFileNumMessage, strlen(dirFileNumMessage));

    if(_o) {
        write(STDERR_FILENO, "\nData saved on file ", 20);
        write(STDERR_FILENO, outputFile, strlen(outputFile));
    } 

    if(_v) {
        write(STDERR_FILENO, "\nExecution records saved on file ", 33);
        write(STDERR_FILENO, getenv("LOGFILENAME"), strlen(getenv("LOGFILENAME")));
    }

    char strExecTime[10], strFinalOutput[40];
    execTimeConverter(strExecTime);
    strcpy(strFinalOutput, "\n");
    strcat(strFinalOutput, "The program took ");
    strcat(strFinalOutput, strExecTime);
    strcat(strFinalOutput, " s to execute.\n\n");
    write(STDERR_FILENO, strFinalOutput, strlen(strFinalOutput));

}

