#include "argumentHandler.h"
#include "forensic.h"

int main(int argc, char *argv[])
{
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