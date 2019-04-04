#include <time.h>
#include "argumentHandler.h"
#include "forensic.h"

//clock_t time0;
struct timespec time0;
bool sigint = false;
int fileNumber = 0;
int dirNumber = 0;

void sigint_handler (int signo) {
    int cenas = signo;
    cenas = cenas+ cenas;
    //printf("\n\nYou clicked on CTRL+C\n\n");
    sigint = true;
}

void siguser1_handler (int signo) {
    int cenas = signo;
    cenas = cenas + cenas;
    fileNumber++;
}

void siguser2_handler (int signo) {
    int cenas = signo;
    cenas = cenas + cenas;
    dirNumber++;
}

int isDirectory(const char *name)
{
    struct stat dir;
    if (lstat(name, &dir))
    {
        return 0;
    }
    return S_ISDIR(dir.st_mode);
}

void execTimeConverter(char str[]) {
    struct timespec time1;
    clock_gettime(CLOCK_REALTIME, &time1);

    long msec = (time1.tv_nsec - time0.tv_nsec) /1000000;
    long sec;
    if(msec < 0) {
        sec = time1.tv_sec - time0.tv_sec -1;
        msec += 1000;
        msec %= 1000;
    } 
    else sec = time1.tv_sec - time0.tv_sec;
        

    sprintf(str, "%ld.%03ld", sec, msec);
}

void realTimeConverter(time_t sec, char str[])
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
    if (pipe(pipeFd) == -1)
    {
        perror("Pipe error");
        return 1;
    }

    pid_t pid = fork();

    //if parent
    if (pid > 0)
    {
        wait(NULL);
        close(pipeFd[WRITE]);
        // catch child's output
        read(pipeFd[READ], readStr, sizeof readStr);
        close(pipeFd[READ]);
    }
    //if child
    else if (pid == 0)
    {
        // redirect output
        dup2(pipeFd[WRITE], STDOUT_FILENO);
        // execute command with filename argument
        execlp(cmd, cmd, filename, NULL);
    }
    else
    {
        perror("Fork error");
        return 1;
    }

    //if file command, erase filename from the beginning
    char *fileStr = "file";
    if (cmd == fileStr)
    {
        snprintf(outputStr, strlen(readStr) - strlen(filename) - 2, "%s", readStr + strlen(filename) + 2);
    }
    //if hash command, erase filename from the end
    else
    {
        //returns sub string from readStr before " " (space)
        strcpy(outputStr, strtok(readStr, " "));
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
    if (lstat(filename, &file_stat) == -1)
        return 1;

    //filename
    write(STDOUT_FILENO, filename, strlen(filename));
    write(STDOUT_FILENO, ",", 1);

    //file type
    if (forkPipeExec(execStr, "file", filename))
        return 1;
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
    realTimeConverter(file_stat.st_mtime, timeStr);
    write(STDOUT_FILENO, timeStr, sizeof timeStr);
    write(STDOUT_FILENO, ",", 1);

    //file modification time
    realTimeConverter(file_stat.st_atime, timeStr);
    write(STDOUT_FILENO, timeStr, sizeof timeStr);

    // digital impressions
    if (_h_md5)
    {
        char md5Str[32];
        if (forkPipeExec(md5Str, "md5sum", filename))
            return 1;
        write(STDOUT_FILENO, ",", 1);
        write(STDOUT_FILENO, md5Str, sizeof md5Str);
    }
    if (_h_sha1)
    {
        char sha1Str[40];
        if (forkPipeExec(sha1Str, "sha1sum", filename))
            return 1;
        write(STDOUT_FILENO, ",", 1);
        write(STDOUT_FILENO, sha1Str, sizeof sha1Str);
    }
    if (_h_sha256)
    {
        char sha256Str[64];
        if (forkPipeExec(sha256Str, "sha256sum", filename))
            return 1;
        write(STDOUT_FILENO, ",", 1);
        write(STDOUT_FILENO, sha256Str, sizeof sha256Str);
    }

    write(STDOUT_FILENO, "\n", 1);

    return 0;
}

int forkdir(const char *dirname)
{
    pid_t pid = fork();

    if (pid > 0)
    {
        wait(NULL);
        return 0;
    }
    else if (pid == 0)
    {
        dirAnalysis(dirname);
        exit(0);
    }
    else
    {
        perror("Fork error");
        return 1;
    }
}

int dirAnalysis(const char *dirname)
{
    DIR *dir = opendir(dirname);
    struct dirent *dirFile;

    while ((dirFile = readdir(dir)) != NULL && !sigint)
    {
        char auxFile[150];

        char *strpoint = ".", *str2point = "..";
        if (!(strcmp(dirFile->d_name, strpoint) && strcmp(dirFile->d_name, str2point)))
        {
            continue;
        }

        //if(isDirectory(dirFile->d_name) && strstr(dirFile->d_name, ".") != NULL)
        //  continue;

        strcpy(auxFile, dirname);
        strcat(auxFile, "/");
        strcat(auxFile, dirFile->d_name);

        if (isDirectory(auxFile))
        {
            if (_r)
            {
                if (forkdir(auxFile))
                    return 1;
            }
            continue;
        }

        fileAnalysis(auxFile);

        if (_v)
        {
            char strreport[200];
            strcpy(strreport, "ANALIZED ");
            strcat(strreport, auxFile);
            reportLog(strreport);
        }
    }

    return 0;
}

void reportLog(const char *report)
{
    // time calculation
    //clock_t time1;
    //while((time1= clock()) == -1)  {}
    //int time_taken = ((double)time1 - time0) / CLOCKS_PER_SEC * 1000;
    char logtime[10], strpid[6];

    // cast from double to c-str
    //sprintf(logtime, "%d", time_taken);
    execTimeConverter(logtime);

    // write time in file
    write(logFd, logtime, strlen(logtime));
    write(logFd, " - ", 3);

    //  cast pid to c-str
    sprintf(strpid, "%d", getpid());

    // write pid in file
    write(logFd, strpid, strlen(strpid));
    write(logFd, " - ", 3);

    // write report in file
    write(logFd, report, strlen(report));
    write(logFd, "\n", 1);
}
