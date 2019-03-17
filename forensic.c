#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

bool _r = false, _h_md5 = false, _h_sha256 = false, _h_sha1 = false, _v = false;
int fd;

void closeFile() {
    dup2(1, STDOUT_FILENO); //necessary?
    close(fd);
}

int argumentInterpreter(int argc, char *argv[]) {
    //if only executable file name called
    if (argc < 2) {
        write(STDERR_FILENO, "usage: forensic [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>", 80);
        return 1;
    }
    for(int i = 1; i < argc; i++) {
        if(argv[i] == "-r") {
            _r = true;
        }
        else if (argv[i] == "-o") {
            i++;
            fd = open(argv[i]);
            dup2(fd, STDOUT_FILENO);
            atexit(closeFile);
        }
        else if (argv[i] == "-v") {
            _v = true;
        }
        else if (argv[i] == "-h") { //question about usage
            
        }
        else {

        }
    }
}


int main(int argc, char *argv[]) {
    if (argumentInterpreter(argc, argv)) exit(1);
}