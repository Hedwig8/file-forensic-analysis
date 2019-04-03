#include <time.h>
#include "argumentHandler.h"
#include "forensic.h"

int SIGINSTALL(){

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

    if(SIGINSTALL()) exit(1);

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

    return 0;
}