// Client side C/C++ program to demonstrate Socket programming
#define _OE_SOCKETS
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int main(int argc, char *argv[])
{
    if(argc==1)
    {
        printf("Usage: ./a.out file [file1] [file2] ...\n");
        exit(0);
    }

    struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    // Creating the socket and handling the error condition if socket creation failed.

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    printf("Starting client...\n");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    // connect to the server address
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("\nConnection Failed \n");
        return -1;
    }   
    
    int i;
    for(i=1; i<argc; i++)
    {
        char* filename= argv[i];

        // Send the request to download the file to the server.
        if(send(sock,filename,strlen(filename),0) < 0)
        {
            perror("Unable to make request");
            close(sock);
            exit(EXIT_FAILURE);
        }

        printf("File %s requested from server\n", filename);

        // Create a file with permissions 664 and assign file descriptor to fd
        int fd = open(filename, O_CREAT | O_WRONLY | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        if (fd == -1)
        {
            // Error handling: If file cannot be opened.
            perror("Couldn't open file");
            exit(EXIT_FAILURE);
        }

        int flag=1;
        int count;
        int file_size = 0;
        count = recv(sock,&file_size,sizeof(file_size),0);
        int fsize = ntohl(file_size);
        int cnt = 0;
        send(sock,"ACK",sizeof("ACK"),0);

        // Receive the contents of file from server
        while ((count = recv(sock,buffer,1024,0)) >= 0)
        {
            cnt+=count;    
            if(count==0)
            {
                flag=0;
                remove(filename);
                printf("Error: Server is not running\n");
                exit(1);
            }

            if(strcmp(buffer,"stopreceiving")==0)
            {
                break;
            }

            if(strcmp(buffer,"Invalid")==0)
            {
                // Error handling: If file requested doesn't exist
                flag=0;
                remove(filename); // Delete the opened file
                printf("%s: File does not exist in the server directory\n",filename);
                break;
            }
            write(fd,buffer,count); // Write to opened local file
            memset(buffer,0,1024);
            if(cnt>=fsize) break;
        }

        if(flag) // Download status
        {
            printf("%s: File successfully downloaded from the server!\n", filename);
        }

        close(fd); // Close file descriptor associated with opened file
    }

    close(sock); // Close file descriptor associated with socket
    return 0;
}
