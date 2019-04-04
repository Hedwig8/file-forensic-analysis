#include "argumentHandler.h"

// argument options
bool _r = false, _h_md5 = false, _h_sha256 = false, _h_sha1 = false, _v = false, _o =false;

//output file descritor
int fd;
int logFd;
char *outputFile;

// getopt function possible arguments
extern char *optarg;
extern int optopt, optind;


void closeFileFd() {
    close(fd);
}

void closeLogFile() {
    close(logFd);
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
                char* logFile = getenv("LOGFILENAME");
                if (logFile == NULL)
                {
                    write(STDERR_FILENO,"\nEnvironment var doesn't exist\n\n",33);
                    return 1;
                }
                else {

                    if ((logFd = open(logFile,O_TRUNC | O_WRONLY)) == -1) 
                    {
                        write(STDERR_FILENO, "\nThe log file doesn't exist\n\n",29);
                        return 1;
                    }
                    atexit(closeLogFile);
                }
				break;

			// -o option, with argument
			case 'o':
                //checks if the argument is an option
                if(optarg[0] == '-') {
                    write(STDERR_FILENO, "Option '-o' requires argument of output file\n", 45);
                    return 1;
                }

				if((fd = open(optarg, O_TRUNC | O_WRONLY)) == -1) {
                    write(STDERR_FILENO, "\nThe output file doesn't exist\n\n", 32);
                    return 1;
                }
                _o = true;
                outputFile = optarg;
                dup2(fd, STDOUT_FILENO);
                atexit(closeFileFd);
				break;

            // -h option, with argument
            case 'h': 
                // checks if the argument is an option
                if(optarg[0] == '-') {
                    write(STDERR_FILENO, "Option '-h' requires argument of output file\n", 45);
                    return 1;
                }

                // checks if the argument has any of the occurences
                if(strstr(optarg, "md5") != NULL) _h_md5 = true;
                if(strstr(optarg, "sha1") != NULL) _h_sha1 = true;
                if(strstr(optarg, "sha256") != NULL) _h_sha256 = true;

                break;

			// errors
			case '?':
				if (optopt == 'o'){ // forgotten argument
					write(STDERR_FILENO, "Option '-o' requires argument of output file\n", 45);
                    return 1;
                }
				else if (optopt == 'h') { //forgotten argument
                    write(STDERR_FILENO, "Option '-h' requires argument of hash calculations\n", 51);
				    return 1;
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