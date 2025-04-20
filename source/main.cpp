#undef main

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

extern "C"
{
    static int g_libnetMemId = -1;
    int sceNetInit();
    int sceNetPoolCreate(const char *, int, int);
    int sceNetPoolDestroy(int);
};

#define NET_HEAP_SIZE (5 * 1024 * 1024)
#define BUF_SIZE 32768

static void terminate()
{
    if (g_libnetMemId != -1)
    {
        sceNetPoolDestroy(g_libnetMemId);
    }
}

int main(int argc, char *argv[])
{
    char buffer[BUF_SIZE];
    in_addr_t in_addr;
    in_addr_t server_addr;
    int filefd;
    int sockfd;
    ssize_t i;
    ssize_t read_return;
    struct hostent *hostent;
    struct sockaddr_in sockaddr_in;
    unsigned short server_port = 9021;

    if (sceNetInit() != 0)
        return -1;

        if ((g_libnetMemId = sceNetPoolCreate("ezremote_client", NET_HEAP_SIZE, 0)) < 0)
    {
        return -1;
    }
    atexit(terminate);

    if (argc > 1)
    {
        printf("Loading %s\n", argv[1]);
    }
    else
    {
        printf("Payload parameter is empty\n");
        return -1;
    }

    filefd = open(argv[1], O_RDONLY);
    if (filefd == -1)
    {
        printf("Failed to open payload to read\n");
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("ELF Loader not started\n");
        return -1;
    }

    /* Prepare sockaddr_in. */
    hostent = gethostbyname("127.0.0.1");
    if (hostent == NULL)
    {
        printf("error: gethostbyname(\"%s\")\n", "127.0.0.1");
        return -1;
    }

    in_addr = inet_addr(inet_ntoa(*(struct in_addr *)*(hostent->h_addr_list)));
    if (in_addr == (in_addr_t)-1)
    {
        printf("error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
        return -1;
    }

    sockaddr_in.sin_addr.s_addr = in_addr;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);
    /* Do the actual connection. */
    if (connect(sockfd, (struct sockaddr *)&sockaddr_in, sizeof(sockaddr_in)) == -1)
    {
        printf("Couldn't connect to ELF loader\n");
        return -1;
    }
    printf("Successfully connected to ELF Loader\n");

    while (1)
    {
        read_return = read(filefd, buffer, BUF_SIZE);
        printf("read %d bytes\n", read_return);
        if (read_return == 0)
            break;
        if (read_return == -1)
        {
            printf("Failed to read from file\n");
            return -1;
        }
        if (write(sockfd, buffer, read_return) == -1)
        {
            printf("Failed to write payload content to ELF loader\n");
            return -1;
        }
    }

    printf("Successfully sent payload\n");

    close(filefd);
    close(sockfd);

    return 0;
}
