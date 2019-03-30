#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#include "argumentHandler.h"

// option arguments
//int fd;

// getopt function possible arguments
//extern int optind;



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
        //struct timespec birth;
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
            execlp(cmd, cmd, filename, NULL);
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

        //file modification time
        //timeConverter(birth.tv_sec, str);
        write(STDOUT_FILENO, str, sizeof str);
        write(STDOUT_FILENO, "\n", 1);

        //md5, sha1, sha256, with execs
        if (_h_md5) {
            if (fork() > 0) {
                wait(NULL);
                write(STDOUT_FILENO, ",", 1);
            }
            else {
                const char *cmd = "md5sum";
                execlp(cmd, cmd, filename, NULL);
            }
        }
        if (_h_sha1) {
            if (fork() > 0) {
                wait(NULL);
                write(STDOUT_FILENO, ",", 1);
            }
            else {
                const char *cmd = "sha1sum";
                execlp(cmd, cmd, filename, NULL);
            }
        }
        if (_h_sha256) {
            if (fork() > 0) {
                wait(NULL);
                write(STDOUT_FILENO, ",", 1);
            }
            else {
                const char *cmd = "sha256sum";
                execlp(cmd, cmd, filename, NULL);
            }   
        }
    }

    // to be printed
    // file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    return 0;
}