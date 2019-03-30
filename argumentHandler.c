#include "argumentHandler.h"

// argument options
bool _r = false, _h_md5 = false, _h_sha256 = false, _h_sha1 = false, _v = false;

//output file descritor
int fd;

// getopt function possible arguments
extern char *optarg;
extern int optopt, optind;


void closeFile() {
    close(fd);
}

int argumentHandler(int argc, char *argv[]) {
    //if only executable file name called
    if (argc < 2 || argc > 8) {
        write(STDERR_FILENO, "\nusage: forensic [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n\n", 82);
        return 1;
    }

    char option;
    //iterating through the options
    while ((option = getopt (argc, argv, "rh:vo:")) != -1)
	{
		switch (option)
		{
			// -r option 
			case 'r':
				_r = true;
				break;

			// -v option
			case 'v':
				_v = true;
				break;

			// -o option, with argument
			case 'o':
                if(optarg[0] == '-') {
                    write(STDERR_FILENO, "Option '-o' requires argument of output file\n", 45);
                    return 1;
                }
				if((fd = open(optarg, O_TRUNC | O_WRONLY)) == -1) {
                    write(STDERR_FILENO, "\nThe output file doesn't exist\n\n", 32);
                    return 1;
                }
                dup2(fd, STDOUT_FILENO);
                atexit(closeFile);
				break;

            // -h option, with argument
            case 'h': 

                //QUESTIONS

                break;



			// errors
			case '?':
				if (optopt == 'o'){ // forgotten argument
					write(STDERR_FILENO, "Option '-o' requires argument of output file\n", 45);
                    return 1;
                }
				else if (optopt == 'h') {
                    //needs missing argument error?
				    
                }
			    else {
			        write(STDERR_FILENO, "That option doesn't exist\n", 26);
                    write(STDERR_FILENO, "\nusage: forensic [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n\n", 82);
				    return 1;
                }
		}
	}
    if(optind == argc) {
        write(STDERR_FILENO, "file|dir argument missing\n", 26);
        write(STDERR_FILENO, "\nusage: forensic [-r] [-h [md5[,sha1[,sha256]]] [-o <outfile>] [-v] <file|dir>\n\n", 82);
        return 1;
    }
    return 0;
}