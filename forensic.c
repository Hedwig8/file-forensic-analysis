#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

// option arguments
bool _r = false, _h_md5 = false, _h_sha256 = false, _h_sha1 = false, _v = false;
int fd;

// getopt function possible arguments
extern char *optarg;
extern int optopt, optind;

void closeFile() {
    close(fd);
}

int isDirectory(const char* name) {
    struct stat dir;
    if(lstat(name, &dir)) {
        return 0;
    }
    return S_ISDIR(dir.st_mode);
}

void timeConverter(time_t sec, char str[]) {

    struct tm time;
    localtime_r(&sec, &time);
    strftime(str, sizeof("dd-mm-yyyyThh:mm:ss"), "%FT%H:%M:%S", &time);
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


int main(int argc, char *argv[]) {
    if (argumentHandler(argc, argv)) //0 if OK
        exit(1);

    // opens 
    //char path[100];
    //strcpy(path, getenv("PWD"));
    //strcat(path, "/");
    //strcat(path, argv[optind]);
    //printf("%s %d %d\n", path, isDirectory(argv[optind]), isDirectory(path));

    //DIR *dir1 = opendir(argv[optind]);
    //DIR *dir2 = opendir(path);

    //printf("\n%p %p", dir1, dir2);
    char *filename = argv[optind];

    if(isDirectory(filename)) {
        //DIR *dir = opendir(argv[optind]);
        printf("1");
        write(STDOUT_FILENO, filename, strlen(filename));
    }
    else {
        char str[19];
        struct stat file_stat;
        struct timespec birth;
        lstat(filename, &file_stat);
        //birth = file_stat.st_birthtimespec;

        //filename
        write(STDOUT_FILENO, filename, sizeof filename);
        write(STDOUT_FILENO, ",", 1);

        //writes file_type with exec file filename
        /*
        if (S_ISBLK(file_stat.st_mode)) {write(STDOUT_FILENO, "block special file,",19);}
        else if (S_ISCHR(file_stat.st_mode)) {write(STDOUT_FILENO, "character special file,",23);}
        else if (S_ISDIR(file_stat.st_mode)) {write(STDOUT_FILENO, "directory,",9);}
        else if (S_ISFIFO(file_stat.st_mode)) {write(STDOUT_FILENO,"pipe or FIFO special file,",26);}
        else if (S_ISREG(file_stat.st_mode)) {write(STDOUT_FILENO, "regular file,",13);}
        else if (S_ISLNK(file_stat.st_mode)) {write(STDOUT_FILENO, "symbolic link,",14);}
        else {write(STDOUT_FILENO, "undefined,",10);}
        */
        if (fork() > 0) {
            wait(NULL);
            write(STDOUT_FILENO, ",", 1);
        }
        else {
            const char *cmd = "file";
            char *const argv[] = {filename};
            execlp(cmd, cmd, filename, NULL);
            write(STDOUT_FILENO, ",", 1);
        }

        //file size
        sprintf(str, "%ld", file_stat.st_size);
        write(STDOUT_FILENO, str, strlen(str));
        write(STDOUT_FILENO, ",", 1);

        //file access
        if (S_IRUSR & file_stat.st_uid) 
            write(STDOUT_FILENO, "r", 1);
        if (S_IWUSR & file_stat.st_uid)
            write(STDOUT_FILENO, "w", 1);
        if (S_IXUSR & file_stat.st_uid)
            write(STDOUT_FILENO, "e", 1);
        write(STDOUT_FILENO, ",", 1);
    
        //file created time
        timeConverter(file_stat.st_ctime, str);
        write(STDOUT_FILENO, str, sizeof str);
        write(STDOUT_FILENO, ",", 1);

        //file modification time
        timeConverter(birth.tv_sec, str);
        write(STDOUT_FILENO, str, sizeof str);
        write(STDOUT_FILENO, "\n", 1);

        //md5, sha1, sha256, with execs
    }

    // to be printed
    // file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    return 0;
}