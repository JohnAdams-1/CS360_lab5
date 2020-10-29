#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>     // for dirname()/basename()
#include <time.h> 

#define MAX 256
#define BLK 1024

struct sockaddr_in saddr; 
char *serverIP   = "127.0.0.1";
int   serverPORT = 1235;
int   sock;

int init()
{
    int n; 
    printf("========== client init ============/n");
    printf("1. create a socket\n");
    sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (sock < 0) { 
        printf("socket creation failed\n"); 
        exit(0); 
    }
    
    printf("2. fill in server IP=%s, port number=%d\n", serverIP, serverPORT);
    bzero(&saddr, sizeof(saddr)); 
    saddr.sin_family = AF_INET; 
    saddr.sin_addr.s_addr = inet_addr(serverIP); 
    saddr.sin_port = htons(serverPORT); 
  
    printf("3. connect to server\n");
    if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    }
    printf("4. connected to server OK\n");
    printf("==========init done==========\n");

}

char** parseInput(char *input)
{
    int count = 0;
    char *str = NULL;
    char *tmpStr = NULL;
    // Allocate char* inputArr[20]
    char** inputArr = (char**)malloc(sizeof(char*)*20);
    str = strtok(input, " ");
    while(str)
    {
        tmpStr = (char *)malloc(sizeof(char)*strlen(str));
        strcpy(tmpStr, str);
        inputArr[count] = tmpStr;
        count++;
        str = strtok(NULL, " ");
    }
    inputArr[count] = NULL;
    return inputArr;
}

  
int main(int argc, char *argv[], char *env[]) 
{ 
    int  n, c, i=0;
    char line[MAX], ans[MAX], copy[MAX], cwd[128];
    char *token;
    char **inputArray;

    FILE *fp, *gp;
    DIR *dir;
    struct dirent *dir_entry;

    init();
  
    while (1){
      printf("input a line : ");
      fgets(line, MAX, stdin);
      line[strlen(line)-1] = 0;        // kill <CR> at end
      if (line[0]==0)                  // exit if NULL line
         exit(0);


    strcpy(copy, line);
    inputArray = parseInput(copy);

    if(!strcmp(inputArray[0], "lcat"))
    {
        if(inputArray[1])
        {
            fp = fopen(inputArray[1], "r");
            if(fp != NULL)
            {
                while((c = fgetc(fp)) != EOF)
                {
                    putchar(c);
                }
            }
            fclose(fp);
        }
    }
    else if(!strcmp(inputArray[0], "lpwd"))
    {
        printf("printing cwd\n");
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
    }
    else if(!strcmp(inputArray[0], "lls"))
    {
        printf("doing lls\n");
        //if no path given, ls off cwd, else ls off pathname
        if(inputArray[1] == NULL)
        {
            printf("printing cwd\n");
            dir = opendir(".");
            while((dir_entry = readdir(dir)) != NULL)
                printf("%s ", dir_entry->d_name);
            closedir(dir);
            printf("\n");
        }
        else
        {
            printf("printing directory\n");
            dir = opendir(inputArray[1]);
            while((dir_entry = readdir(dir)) != NULL)
                printf("%s ", dir_entry->d_name);
            closedir(dir);
            printf("\n");
        }
    }
    else if(!strcmp(inputArray[0], "lcd"))
    {
        //if directory name present, chdir
        if(strcmp(inputArray[1], ""))
        {
            printf("Changing directory to %s", inputArray[1]);
            chdir(inputArray[1]);
            //change current working directory
            getcwd(cwd, 128);
        }
        else
            printf("lcd Error: No Directory Specified\n");
    }
    else if(!strcmp(inputArray[0], "lmkdir"))
    {
        //check if directory name present, if so mkdir
        if(!strcmp(inputArray[1], ""))
            printf("missing directory name\n");
        else
        {
            printf("making directory\n");
            mkdir(inputArray[1], 0775);
        }
    }
    else if(!strcmp(inputArray[0], "lrmdir"))
    {
        printf("removing %s\n", inputArray[1]);
        //check if directory name present, if so rmdir
        if(!strcmp(inputArray[1], ""))
            printf("missing directory name\n");
        else
            rmdir(inputArray[1]);
    }
    else if(!strcmp(inputArray[0], "lrm"))
    {
        //check if filename present, if so unlink
        if(!strcmp(inputArray[1], ""))
            printf("missing file name\n");
        else
            unlink(inputArray[1]);
    }
    else
    {
      // Send ENTIRE line to server
      n = write(sock, line, MAX);
      printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

      // Read a line from sock and show it
      bzero(ans, MAX);
      n = read(sock, ans, MAX);
      printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
    }
    }
}

