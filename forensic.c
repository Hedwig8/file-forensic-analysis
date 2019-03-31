#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>

#include "argumentHandler.h"

#define READ 0
#define WRITE 1


int isDirectory(const char *name)
{
    struct stat dir;
    if (lstat(name, &dir))
    {
        return 0;
    }
    return S_ISDIR(dir.st_mode);
}

void timeConverter(time_t sec, char str[])
{

    struct tm time;
    localtime_r(&sec, &time);
    strftime(str, sizeof("dd-mm-yyyyThh:mm:ss"), "%FT%H:%M:%S", &time);
}

int forkPipeExec(char outputStr[], const char *cmd, const char *filename)
{
    char readStr[255];

    //pipe to catch child's output
    int pipeFd[2];
    if(pipe(pipeFd) == -1) {
        perror("Pipe error");
        return 1;
    }

    pid_t pid = fork();

    //if parent
    if (pid > 0) {
        close(pipeFd[WRITE]);
        // catch child's output
        read(pipeFd[READ], readStr, sizeof readStr);    
    }
    //if child
    else if( pid == 0) {
        // redirect output
        dup2(pipeFd[WRITE], STDOUT_FILENO);             
        // execute command with filename argument
        execlp(cmd, cmd, filename, NULL);           
    }
    else {
        perror("Fork error");
        return 1;
    }


    //if file command, erase filename from the beginning
    char* fileStr = "file";
    if(cmd == fileStr) {
        snprintf(outputStr, strlen(readStr) - strlen(filename)-2, "%s", readStr+strlen(filename)+2);
    }
    //if hash command, erase filename from the end
    else {
        //returns sub string from readStr before " " (space)
        strcpy(outputStr,strtok(readStr, " "));
    }

    strcpy(outputStr, strtok(outputStr, "\n"));
/*
    if(outputAux[strlen(outputAux) -1] == '\n') 
        snprintf(outputStr, strlen(outputAux)-1, "%s", outputAux);
    else  snprintf(outputStr, strlen(outputAux), "%s", outputAux);*/
/*
    if (pid > 0)
    {
        close(fd[WRITE]);
        char readChar;

        if (cmd[0] == 'f')
        {

            do
            {
                read(fd[READ], &readChar, sizeof(char));
            } while (readChar != ':');

            do
            {
                read(fd[READ], &readChar, sizeof(char));
                if (readChar != '\n')
                    write(STDOUT_FILENO, &readChar, 1);
            } while (readChar != '\n');
        }
        else
        {
            do
            {
                read(fd[READ], &readChar, sizeof(char));
                if (readChar != ' ')
                    write(STDOUT_FILENO, &readChar, 1);
            } while (readChar != ' ');
        }
    }
    else if (pid == 0)
    {
        dup2(fd[WRITE], STDOUT_FILENO);
        execlp(cmd, cmd, filename, NULL);
    }
    else
    {
        perror("Fork failed!");
        return 1;
    }*/
    return 0;
}

int fileAnalysis(const char *filename)
{
    char sizeStr[11];
    char timeStr[19];
    char execStr[255];
    
    struct stat file_stat;
    lstat(filename, &file_stat);

    //filename
    write(STDOUT_FILENO, filename, sizeof filename);
    write(STDOUT_FILENO, ",", 1);

    //file type
    forkPipeExec(execStr, "file", filename);
    write(STDOUT_FILENO, execStr, strlen(execStr));
    write(STDOUT_FILENO, ",", 1);

    //file size
    sprintf(sizeStr, "%ld", file_stat.st_size);
    write(STDOUT_FILENO, sizeStr, strlen(sizeStr));
    write(STDOUT_FILENO, ",", 1);

    //file access
    if (S_IRUSR & file_stat.st_mode)
        write(STDOUT_FILENO, "r", 1);
    if (S_IWUSR & file_stat.st_mode)
        write(STDOUT_FILENO, "w", 1);
    if (S_IXUSR & file_stat.st_mode)
        write(STDOUT_FILENO, "x", 1);
    write(STDOUT_FILENO, ",", 1);

    //file created time
    timeConverter(file_stat.st_ctime, timeStr);
    write(STDOUT_FILENO, timeStr, sizeof timeStr);
    write(STDOUT_FILENO, ",", 1);

    //file modification time
    timeConverter(file_stat.st_mtime, timeStr);
    write(STDOUT_FILENO, timeStr, sizeof timeStr);


/*
    if (_h_md5 || _h_sha1 || _h_sha256)
    {
        write(STDOUT_FILENO, ",", 1);
    }*/

    // digital impressions 
    if (_h_md5)
    {
        char md5Str[32];
        write(STDOUT_FILENO, ",", 1);
        forkPipeExec(md5Str, "md5sum", filename);
        write(STDOUT_FILENO, md5Str, sizeof md5Str);
    }
    if (_h_sha1)
    {
        char sha1Str[40];
        write(STDOUT_FILENO, ",", 1);
        forkPipeExec(sha1Str, "sha1sum", filename);
        write(STDOUT_FILENO, sha1Str, sizeof sha1Str);
    }
    if (_h_sha256)
    {
        char sha256Str[64];
        write(STDOUT_FILENO, ",", 1);
        forkPipeExec(sha256Str, "sha256sum", filename);
        write(STDOUT_FILENO, sha256Str, sizeof sha256Str);
    }

    write(STDOUT_FILENO, "\n", 1);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argumentHandler(argc, argv)) //0 if OK
        exit(1);

    char *filename = argv[optind];

    // --------------------- TO BE MOVED TO SPECIFIC FUNCTION
    if (isDirectory(filename))
    {
        //DIR *dir = opendir(argv[optind]);
        printf("1");
        write(STDOUT_FILENO, filename, strlen(filename));
    }
    else
    {
        fileAnalysis(filename);
    }

    // to be printed
    // file_name,file_type,file_size,file_access,file_created_date,file_modification_date,md5,sha1,sha256

    return 0;
}