#include <time.h>
#include <stdlib.h>
#include "argumentHandler.h"
#include "forensic.h"

int signalsInstall(){

    struct sigaction sigint;
    sigint.sa_handler = sigint_handler;
 
    if (sigaction(SIGINT,&sigint,NULL) < 0)
    {
        write(STDERR_FILENO,"Unable to install SIGINT handler\n",32);
        return 1;
    }

    struct sigaction siguser1;
    siguser1.sa_handler = siguser1_handler;
 
    if (sigaction(SIGUSR1,&siguser1,NULL) < 0)
    {
        write(STDERR_FILENO,"Unable to install SIGUSR1 handler\n",34);
        return 1;
    }

    struct sigaction siguser2;
    siguser2.sa_handler = siguser2_handler;
 
    if (sigaction(SIGUSR2,&siguser2,NULL) < 0)
    {
        write(STDERR_FILENO,"Unable to install SIGUSR2 handler\n",34);
        return 1;
    }

    return 0;


}

int main(int argc, char *argv[])
{
    time0 = clock();
    //time(&time0);

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

    //clock_t time1 = clock();
    //clock_t time1;
    //time0 = clock() - time0;
    //time(&time1);
    //char strTimeMessage[50], strtime[10];
    //double total_time = ((double) time1-time0) / CLOCKS_PER_SEC;
    //double total_time = difftime(time1, time0);
    //sprintf(strtime,"%d", total_time);

    //strcpy(strTimeMessage, "\nThe execution took ");
    //strcat(strTimeMessage, strtime);
    //strcat(strTimeMessage, " ms to execute\n\n");


    //write(STDERR_FILENO, strTimeMessage, strlen(strTimeMessage));
    //printf("%f", (double) time0/CLOCKS_PER_SEC);

    return 0;
}