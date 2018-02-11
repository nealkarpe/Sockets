#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define _OE_SOCKETS
#include <unistd.h>

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


int main(int argc, char const *argv[])
{
    int server_fd, new_socket, file_fd;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char filename[128] = {0};
    char *response = "Success";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        // Error handling: socket creation failed
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        // Binding the port and the ip
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening...\n");
    while(1)
    {
        // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0)
        {
            // Error handling: If accepting connection fails
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int filename_size = recv(new_socket, filename, 128, 0); // read information received into the buffer
        filename[filename_size]='\0';

        printf("File requested: %s\n", filename);

        //temp string contains path to file
        char temp[128];
        temp[0]='\0';
        strcpy(temp, "Data/");
        strcat(temp, filename);

        // Opening the file to be sent to the client
        if ((file_fd = open(temp, O_RDONLY)) == -1)
        {
            // Error handling: If file requested doesn't exist with server
            printf("%s: File not found\n", filename);
            send(new_socket,"Invalid", 7, 0);
            close(file_fd); // Close file descriptor associated with the opened file
            close(new_socket); // Close file descriptor associated with the client's socket
            continue;
        }

        memset(buffer,0,1024); // Clears the buffer
        int totalLeft;
        // Reading the file to be sent to the client
        while((totalLeft = read(file_fd, buffer, 1024)) > 0)
        {
            // each iteration maximum 1024 characters can be read from the file
            char* ptr = buffer;
            int count = 0;
            while (count < totalLeft)
            {
                // while there are still characters to be sent
                totalLeft -= count;
                ptr += count;
                if((count = send(new_socket, ptr, totalLeft, 0)) < 0) // Send the contents of the file
                {
                    // Error handling: If send fails
                    perror("Error sending");
                    exit(1);
                }
            }
        }

        close(file_fd); // Close file descriptor of the file read

        printf("%s: File sent successfully!\n", filename);

        close(new_socket); //Close the file descriptor of the socket for this connection
    }

    return 0;
}
