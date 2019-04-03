#include "argumentHandler.h"
#include "forensic.h"

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
    
    return 0;
}

int fileAnalysis(const char *filename)
{
    char sizeStr[11];
    char timeStr[19];
    char execStr[255];
    
    struct stat file_stat;
    if(lstat(filename, &file_stat) == -1) return 1;

    //filename
    write(STDOUT_FILENO, filename, strlen(filename));
    write(STDOUT_FILENO, ",", 1);

    //file type
    if(forkPipeExec(execStr, "file", filename)) return 1;
    write(STDOUT_FILENO, execStr, strlen(execStr));
    write(STDOUT_FILENO, ",", 1);

    //file size
    sprintf(sizeStr, "%ld", file_stat.st_size);
    write(STDOUT_FILENO, sizeStr, strlen(sizeStr));
    write(STDOUT_FILENO, ",", 1);

    //file access permissions
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

    // digital impressions 
    if (_h_md5)
    {
        char md5Str[32];
        if(forkPipeExec(md5Str, "md5sum", filename)) return 1;
        write(STDOUT_FILENO, ",", 1);
        write(STDOUT_FILENO, md5Str, sizeof md5Str);
    }
    if (_h_sha1)
    {
        char sha1Str[40];
        if(forkPipeExec(sha1Str, "sha1sum", filename)) return 1;
        write(STDOUT_FILENO, ",", 1);
        write(STDOUT_FILENO, sha1Str, sizeof sha1Str);
    }
    if (_h_sha256)
    {
        char sha256Str[64];
        if(forkPipeExec(sha256Str, "sha256sum", filename)) return 1;
        write(STDOUT_FILENO, ",", 1);
        write(STDOUT_FILENO, sha256Str, sizeof sha256Str);
    }

    write(STDOUT_FILENO, "\n", 1);

    return 0;
}

int forkdir (const char* dirname) 
{
    pid_t pid = fork();

    if (pid > 0) {
        wait(NULL);
        return 0;
    } 
    else if (pid == 0) {
        dirAnalysis(dirname);
        exit(1);
    }
    else {
        perror("Fork error");
        return 1;
    }
}

int dirAnalysis(const char* dirname) {

    struct dirent *dirFile;
    DIR *dir = opendir(dirname);

    while((dirFile = readdir(dir)) != NULL) {
        char auxFile[150];
        strcpy(auxFile,dirname);
        strcat(auxFile,"/");
        strcat(auxFile,dirFile->d_name);

        if(isDirectory(auxFile)) 
        {
            if (_r) {
               
                if (strstr(auxFile,".") == NULL) {
                    
                    if(forkdir(auxFile)) return 1;
                }
            }
        }
        fileAnalysis(auxFile);
    }

    return 0;
}
