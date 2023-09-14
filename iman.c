#include "headers.h"

void iman_func(char **token)
{
    char command_name[100];
    strcpy(command_name, token[1]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        return;
    }

    struct hostent *he = gethostbyname("man.he.net");
    if (he == NULL)
    {
        perror("DNS resolution failed");
        close(sockfd);
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        close(sockfd);
        return;
    }

    char request[1024];
    snprintf(request, sizeof(request), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", command_name);

    if (send(sockfd, request, strlen(request), 0) == -1)
    {
        perror("Send failed");
        close(sockfd);
        return;
    }

    char buffer[10000];
    int total_bytes_received = 0;
    int bytes_received;

    while ((bytes_received = recv(sockfd, buffer + total_bytes_received, sizeof(buffer) - total_bytes_received, 0)) > 0)
    {
        total_bytes_received += bytes_received;
    }
    printf("%s", buffer);
    // Parsing the buffer starting from "NAME" and ending at "COPYRIGHT"
    char *start = strstr(buffer, "NAME\n");
    char *end = strstr(buffer, "COPYRIGHT");
    if (start == NULL || end == NULL)
    {
        printf("No manual entry for %s\n", command_name);
        close(sockfd);
        return;
    }
    end -= 2;
    char *temp = start;
    while (temp != end)
    {
        if (*temp == '\n')
        {
            printf("\n");
        }
        else
        {
            printf("%c", *temp);
        }
        temp++;
    }
    printf("\n");

    // Close the socket
    close(sockfd);
    return;
}