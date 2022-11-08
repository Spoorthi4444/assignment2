// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>
#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    printf("execve=0x%p\n", execve);
    pid_t current_pid, parent_pid;
    const char *nobody = "nobody";
    struct passwd *nobody_structure;
    uid_t nobody_pw_uid;
    int return_value;
    current_pid = getuid();
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    // socket creation ends here
    
    printf("Processing the data from client starts here- Implementing Privilage Seperation\n");
    current_pid = fork();
    
    if (current_pid == 0)
    {
    	printf("This is a child process\n");
    	nobody_structure = getpwnam(nobody);
    	if(nobody_structure == NULL)
    	{
    		printf("Matching entry is not found\n");
    		return 0;
    	}
    	nobody_pw_uid = nobody_structure->pw_uid;
    	printf("nobody_pw_uid is : %d\n", nobody_pw_uid);
    	return_value = setuid(nobody_pw_uid);
    	printf("return value of setuid is : %d\n", return_value);
    	if (nobody_pw_uid == -1)
    	{
    		printf("Error while dropping privilage\n");
    		return 0;
    	}        	
    
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    
   }
   else if(current_pid > 0)
   {
   	wait(NULL);
   	printf("This is a parent process\n");
   }
   else
   {
   	printf("parent process:Child creation with for failed\n");
   }
   return 0;
}
