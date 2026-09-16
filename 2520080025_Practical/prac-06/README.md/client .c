#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define CLIENT_FIFO "client_to_server"
#define SERVER_FIFO "server_to_client"

int main(int argc, char *argv[])
{
    char message[200];
    char response[300];

    /* Get message from command line */
    if (argc > 1)
    {
        snprintf(message, sizeof(message), "%s", argv[1]);
    }
    else
    {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        message[strcspn(message, "\n")] = '\0';
    }

    /* Open client FIFO for writing */
    int write_fd = open(CLIENT_FIFO, O_WRONLY);

    if (write_fd == -1)
    {
        perror("open client FIFO");
        exit(EXIT_FAILURE);
    }

    /* Send message */
    write(write_fd, message, strlen(message) + 1);

    close(write_fd);

    printf("Message sent: %s\n", message);

    /* Open server FIFO for reading */
    int read_fd = open(SERVER_FIFO, O_RDONLY);

    if (read_fd == -1)
    {
        perror("open server FIFO");
        exit(EXIT_FAILURE);
    }

    /* Receive server response */
    ssize_t bytes = read(read_fd, response, sizeof(response) - 1);

    if (bytes > 0)
    {
        response[bytes] = '\0';
        printf("Server response: %s\n", response);
    }

    close(read_fd);

    return 0;
}
