/// Server side C/C++ program to demonstrate Socket programming
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
    if (argc == 1)
    {
        int server_fd;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        const char *nobody = "nobody";
        struct passwd *nobody_structure;
        int return_value;

        pid_t current_pid, parent_pid;
        uid_t nobody_pw_uid;
        current_pid = getpid();

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address,
                 sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        

        printf("Processing the data from client starts here- Implementing Privilage Seperation\n");

        current_pid = fork();

        if (current_pid == 0)
        {
            printf("This is a child process\n");
            printf("\nCurrently reading from the client \n");

            nobody_structure = getpwnam(nobody);
            if (nobody_structure == NULL)
            {
                printf("Matching entry is not found\n");
				return 0;
            }
    	nobody_pw_uid = nobody_structure->pw_uid;
    	printf("nobody_pw_uid is : %d\n", nobody_pw_uid);
    	return_value = setuid(nobody_pw_uid);
    	printf("return value of setuid is : %d\n", return_value);
    	if (return_value == -1)
    	{
    		printf("Error while dropping privilage\n");
    		return 0;
    	}

            char fd_arg[10];
            char port_arg[10];
            snprintf(fd_arg, 10, "%d", server_fd);
            snprintf(port_arg, 10, "%d", PORT);

            char *args[] = {fd_arg, port_arg, NULL};

            execv("./server", args);
        }
        else if (current_pid > 0)
        {
            wait(NULL);
            printf("This is a parent process\n");
        }
        else
        {
            printf("parent process:Child creation with fork failed\n");
            _exit(2);
        }
        
        return 0;
    }
    else
    {
        int new_socket, valread;
        struct sockaddr_in address;
        char buffer[1024] = {0};
        int addrlen = sizeof(address);
        char *hello = "Hello from server";

        int server_fd = atoi(argv[0]);
        int port = atoi(argv[1]);

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        valread = read(new_socket, buffer, 1024);
        printf("\nRead %d bytes: %s\n", valread, buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("\nHello message sent\n");
        printf("\nChild -> \t present_user_id: %d\n", getuid());
    }
}
