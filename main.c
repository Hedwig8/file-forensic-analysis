#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include "argumentHandler.h"
#include "forensic.h"

int signalsInstall(){

    struct sigaction sigint;
    sigint.sa_handler = sigint_handler;
    sigemptyset(&sigint.sa_mask);
    sigint.sa_flags = 0;
 
    if (sigaction(SIGINT,&sigint,NULL) < 0)
    {
        write(STDERR_FILENO,"Unable to install SIGINT handler\n",32);
        return 1;
    }

    struct sigaction sigusr1;
    sigusr1.sa_handler = siguser1_handler;
    sigemptyset(&sigusr1.sa_mask);
    sigusr1.sa_flags = 0;
 
    if (sigaction(SIGUSR1,&sigusr1,NULL) < 0)
    {
        write(STDERR_FILENO,"Unable to install SIGUSR1 handler\n",34);
        return 1;
    }

    struct sigaction sigusr2;
    sigusr2.sa_handler = siguser2_handler;
    sigemptyset(&sigusr2.sa_mask);
    sigusr2.sa_flags = 0;
 
    if (sigaction(SIGUSR2,&sigusr2,NULL) < 0)
    {
        write(STDERR_FILENO,"Unable to install SIGUSR2 handler\n",34);
        return 1;
    }

    return 0;


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

int main(int argc, char *argv[])
{
    while((clock_gettime(CLOCK_REALTIME, &time0)) == -1 ) {}

    if(signalsInstall()) exit(1);

    if (argumentHandler(argc, argv)) //0 if OK
        exit(1);

    char *name = argv[optind];

    if (isDirectory(name))
    {
        if(dirAnalysis(name)) return 1;
    }
    else
    {
        if(fileAnalysis(name)) return 1;
    }

    finalMessages();

    return 0;
}