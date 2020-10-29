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

#define MAX   256
#define BLK  1024

int server_sock, client_sock;
char *serverIP = "127.0.0.1";      // hardcoded server IP address
int serverPORT = 1235;             // hardcoded server port number

struct sockaddr_in saddr, caddr, naddr;   // socket addr structs

int r, length, n;
char * token;


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

int init()
{
    printf("============== server init start =================\n");
    printf("1. create a socket\n");
    server_sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_sock < 0) { 
        printf("socket creation failed\n"); 
        exit(0); 
    }
    
    printf("2. fill in server IP and port number\n");
    bzero(&saddr, sizeof(saddr)); 
    saddr.sin_family = AF_INET; 
     saddr.sin_addr.s_addr = inet_addr(serverIP);
    saddr.sin_port = htons(serverPORT);
    
    printf("3. bind socket to server\n");
    if ((bind(server_sock, (struct sockaddr *)&saddr, sizeof(saddr))) != 0) { 
        printf("socket bind failed\n"); 
        exit(0); 
    }
    printf("4. server listen with queue size = 5\n");
    if ((listen(server_sock, 5)) != 0) { 
        printf("Listen failed\n"); 
        exit(0); 
    }
    printf("5. server at IP=%s port=%d\n", serverIP, serverPORT);
    length = sizeof(naddr);
    r = getsockname(server_sock,(struct sockaddr *)&naddr, &length);
    if(r < 0)
    {
        printf("get socketname error\n");
        exit(4);
    }

    serverPORT = ntohs(naddr.sin_port);
    printf("    Port=%d\n", serverPORT);

    printf("5: server is listening ..... \n");
    listen(server_sock, 5);
    printf("================ init done ===================\n");

}
  
int main() 
{
    int n, length;
    char *hostname;
    char **input;
    double arg1, arg2, sum;
    char line[MAX], copy[MAX], cwd[128];

    DIR *dir;
    struct dirent *ent;
    
    init();  

    while(1){
       printf("server: try to accept a new connection\n");
       length = sizeof(caddr);
       client_sock = accept(server_sock, (struct sockaddr *)&caddr, &length);
       if (client_sock < 0){
          printf("server: accept error\n");
          exit(1);
       }
 
       printf("server: accepted a client connection from\n");
       printf("-----------------------------------------------\n");
       printf("    IP=%s  port=%d\n", "127.0.0.1", ntohs(caddr.sin_port));
       printf("-----------------------------------------------\n");

       // Processing loop
       while(1){
         printf("server ready for next request ....\n");
         n = read(client_sock, line, MAX);
         if (n==0){
           printf("server: client died, server loops\n");
           close(client_sock);
           break;
         }


         line[n]=0;
         printf("server: read  n=%d bytes; line=[%s]\n", n, line);

               strcpy(copy, line);
      input = parseInput(copy);

      if(!strcmp(input[0], "pwd"))
      {
        printf("printing pwd\n");
        getcwd(cwd, sizeof(cwd));
        printf("cwd is: %s\n", cwd);
        // if (getcwd(cwd, sizeof(cwd)) != NULL)
        //     printf(stdout, "Current working dir: %s\n", cwd);
      }
      else if (!strcmp(input[0], "ls"))
      {
        printf("doing ls\n");
        //if no path given, ls off cwd, else ls off pathname
        if(input[1] == NULL)
        {
            printf("printing cwd\n");
            dir = opendir(".");
            while((ent = readdir(dir)) != NULL)
                printf("%s ", ent->d_name);
            closedir(dir);
            printf("\n");
        }
        else
        {
            printf("printing directory\n");
            dir = opendir(input[1]);
            while((ent = readdir(dir)) != NULL)
                printf("%s ", ent->d_name);
            closedir(dir);
            printf("\n");
        }
      }
      else if (!strcmp(input[0], "cd"))
      {
        if(strcmp(input[1], ""))
        {
            printf("Changing directory to %s", input[1]);
            chdir(input[1]);
            //change current working directory
            getcwd(cwd, 128);
        }
        else
            printf("lcd Error: No Directory Specified\n");
      }
      else if (!strcmp(input[0], "mkdir"))
      {
        mkdir(input[1], 0755);
      }
      else if (!strcmp(input[0], "rmdir"))
      {
        rmdir(input[1]);
      }
      else if (!strcmp(input[0], "rm"))
      {
        unlink(input[1]);
      }
      else if (!strcmp(input[0], "get"))
      {
        //   if(getpathname())
        //   {
        //       n = read(server_sock, input, MAX);
        //   }
      }
      else if (!strcmp(input[0], "put"))
      {
      }

         strcat(line, " ECHO");
         // send the echo line to client 
         n = write(client_sock, line, MAX);

         printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
       }
    }
}


