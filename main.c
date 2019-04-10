#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "argumentHandler.h"
#include "forensic.h"
#include "utils.h"

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