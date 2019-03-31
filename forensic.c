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

int forkPipeExec(char* cmd, char* filename) {
        int fd[2];
        pipe(fd);
        int v;

        v = fork();

        if (v > 0) {
            close(fd[WRITE]);
            char red;

            if (cmd[0] == 'f') {

            do {
            read(fd[READ], &red,sizeof(char));
            } while (red != ':');

            do {
                read(fd[READ], &red,sizeof(char));
                if (red != '\n')
                    write(STDOUT_FILENO, &red, 1); 
            } while (red != '\n');

            }
            else {
                do {
                    read(fd[READ], &red,sizeof(char));
                    if (red != ' ')
                        write(STDOUT_FILENO, &red, 1);
                } while (red != ' ');
            }
        }
        else if (v == 0){
            dup2(fd[WRITE], STDOUT_FILENO);
            execlp(cmd, cmd, filename, NULL);
        }
        else {
            perror("Fork failed!");
        }
        return 0;
}

int main(int argc, char *argv[]) {
    if (argumentHandler(argc, argv)) //0 if OK
        exit(1);

    char *filename = argv[optind];


    // --------------------- TO BE MOVED TO SPECIFIC FUNCTION
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
        /*const char *cmd = "file";
        ifconst char *cmd = "file"; {write(STDOUT_FILENO, "block special file,",19);}
        elconst char *cmd = "file";ode)) {write(STDOUT_FILENO, "character special file,",23);}
        elconst char *cmd = "file";ode)) {write(STDOUT_FILENO, "directory,",9);}
        elconst char *cmd = "file";mode)) {write(STDOUT_FILENO,"pipe or FIFO special file,",26);}
        elconst char *cmd = "file";ode)) {write(STDOUT_FILENO, "regular file,",13);}
        elconst char *cmd = "file";ode)) {write(STDOUT_FILENO, "symbolic link,",14);}
        else {write(STDOUT_FILENO, "undefined,",10);}
        */



       // -------------------- TO BE MOVED/SUBST FOR FUNCTION
       /*
        int fd[2];
        pipe(fd);
        int v;

        v = fork();

        if (v > 0) {
            close(fd[WRITE]);
            char red;

            do {
            read(fd[READ], &red,sizeof(char));
            } while (red != ':');

            do {
                read(fd[READ], &red,sizeof(char));
                if (red != '\n')
                    write(STDOUT_FILENO, &red, 1); 
            } while (red != '\n');

            write(STDOUT_FILENO, ",", 1);
        }
        else if (v == 0){
            dup2(fd[WRITE], STDOUT_FILENO);
            const char *cmd = "file";
            execlp(cmd, cmd, filename, NULL);
        }
        else {
            perror("Fork failed!");
        }
        */

        //file type
        forkPipeExec("file",filename);
        write(STDOUT_FILENO, ",", 1);

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
        if (_h_md5|_h_sha1|_h_sha256) {
            write(STDOUT_FILENO, ",", 1); 
        }


        // -------------------- TO BE MOVED/SUBST FOR FUNCTION
        //md5, sha1, sha256, with execs
        if (_h_md5) {
            forkPipeExec("md5sum",filename);
            if (_h_sha1 | _h_sha256)
                write(STDOUT_FILENO, ",", 1);   
        }
        if (_h_sha1) {
            forkPipeExec("sha1sum",filename);
            if (_h_sha256)
                write(STDOUT_FILENO, ",", 1);
        }
        if (_h_sha256) {
            forkPipeExec("sha256sum",filename);  
        }

        write(STDOUT_FILENO, "\n", 1);
    }

    // to be printed
    // file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    return 0;
}