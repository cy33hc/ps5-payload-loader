#undef main

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <sys/sysctl.h>

using namespace std;

#define BUF_SIZE 32768
#define MAX_WAIT_TIME 60000000

typedef struct notify_request
{
    char useless1[45];
    char message[3075];
} notify_request_t;

extern "C"
{
    int sceKernelSendNotificationRequest(int, notify_request_t *, size_t, int);
}

struct DirEntry
{
    char path[PATH_MAX];
    char name[PATH_MAX];
    uint64_t datetime;

    friend bool operator<(DirEntry const &a, DirEntry const &b)
    {
        return strcmp(a.name, b.name) < 0;
    }

    static int DirEntryComparator(const void *v1, const void *v2)
    {
        const DirEntry *p1 = (DirEntry *)v1;
        const DirEntry *p2 = (DirEntry *)v2;
        return strcasecmp(p1->name, p2->name);
    }

    static void Sort(std::vector<DirEntry> &list)
    {
        qsort(&list[0], list.size(), sizeof(DirEntry), DirEntryComparator);
    }

};

static bool wait_close_disc_player = false;

static uint64_t get_tick()
{
    static struct timeval tick;
    gettimeofday(&tick, NULL);
    return tick.tv_sec * 1000000 + tick.tv_usec;
}

void notify(const char *fmt, ...)
{
    notify_request_t req;
    va_list args;

    bzero(&req, sizeof req);
    va_start(args, fmt);
    if (wait_close_disc_player)
        vsnprintf(req.message, sizeof req.message, fmt, args);
    else
        printf(fmt, args);
    va_end(args);

    sceKernelSendNotificationRequest(0, &req, sizeof req, 0);
}

int send_elf(const string &elf_file, bool del_file)
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

    filefd = open(elf_file.c_str(), O_RDONLY);
    if (filefd == -1)
    {
        printf("Failed to open payload to read\n");
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Failed to create socket\n");
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
        printf("read %lu bytes\n", read_return);
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

    notify("Successfully sent payload\n");

    close(filefd);
    close(sockfd);

    if (del_file)
        unlink(elf_file.c_str());

    return 0;
}

/**
 * Fint the pid of a process with the given name.
 **/
int find_process(const string &process_name)
{
    int mib[4] = {1, 14, 8, 0};
    pid_t mypid = getpid();
    pid_t pid = -1;
    size_t buf_size;
    uint8_t *buf;

    if (sysctl(mib, 4, 0, &buf_size, 0, 0))
    {
        printf("error: sysctl");
        return -1;
    }

    if (!(buf = (uint8_t*)malloc(buf_size)))
    {
        printf("error: malloc");
        return -1;
    }

    if (sysctl(mib, 4, buf, &buf_size, 0, 0))
    {
        printf("error: sysctl");
        free(buf);
        return -1;
    }

    for (uint8_t *ptr = buf; ptr < (buf + buf_size);)
    {
        int ki_structsize = *(int *)ptr;
        pid_t ki_pid = *(pid_t *)&ptr[72];
        char *ki_tdname = (char *)&ptr[447];

        ptr += ki_structsize;
        if (strcmp(ki_tdname, process_name.c_str()) == 0)
        {
            free(buf);
            return 0;
        }
    }

    free(buf);

    return 1;
}

void mkdir(const std::string &ppath, bool prev)
{
    std::string path = ppath;
    if (!prev)
    {
        path.push_back('/');
    }
    auto ptr = path.begin();
    while (true)
    {
        ptr = std::find(ptr, path.end(), '/');
        if (ptr == path.end())
            break;

        char last = *ptr;
        *ptr = 0;
        int err = mkdir(path.c_str(), 0777);
        *ptr = last;
        ++ptr;
    }
}

bool copy(const std::string &from, const std::string &to)
{
    mkdir(to, true);
    if (from.compare(to) == 0)
        return true;

    FILE *src = fopen(from.c_str(), "rb");
    if (!src)
    {
        return false;
    }

    struct stat file_stat = {0};
    if (stat(from.c_str(), &file_stat) != 0)
    {
        return false;
    }

    uint64_t bytes_to_copy = file_stat.st_size;

    FILE *dest = fopen(to.c_str(), "wb");
    if (!dest)
    {
        fclose(src);
        return false;
    }

    size_t bytes_read = 0;
    uint64_t bytes_copied = 0;
    const size_t buf_size = 0x10000;
    unsigned char *buf = new unsigned char[buf_size];

    do
    {
        bytes_read = fread(buf, sizeof(unsigned char), buf_size, src);
        if (bytes_read < 0)
        {
            delete[] buf;
            fclose(src);
            fclose(dest);
            return false;
        }

        size_t bytes_written = fwrite(buf, sizeof(unsigned char), bytes_read, dest);
        if (bytes_written != bytes_read)
        {
            delete[] buf;
            fclose(src);
            fclose(dest);
            return false;
        }

        bytes_copied += bytes_read;
    } while (bytes_copied < bytes_to_copy);

    delete[] buf;
    fclose(src);
    fclose(dest);
    return true;
}

vector<DirEntry> list_dir(const string &path)
{
    vector<DirEntry> filelist;

    DIR *fd = opendir(path.c_str());
    if (fd == NULL)
    {
        return filelist;
    }

    while (true)
    {
        struct dirent *dirent;
        DirEntry entry;

        dirent = readdir(fd);
        if (dirent == NULL)
        {
            closedir(fd);
            return filelist;
        }
        else
        {
            if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
            {
                continue;
            }

            snprintf(entry.name, sizeof(entry.name)-1, "%s", dirent->d_name);
            snprintf(entry.path, sizeof(entry.path)-1, "%s/%s", path.c_str(), dirent->d_name);
            struct stat file_stat;
            stat(entry.path, &file_stat);
            entry.datetime = file_stat.st_mtim.tv_sec * 1000000000 + file_stat.st_mtim.tv_nsec;
            filelist.push_back(entry);
        }
    }
    closedir(fd);

    return filelist;
}

int main(int argc, char *argv[])
{
    uint64_t start_time;
    uint64_t delta_time;
    int ret;

#if !defined(JAR_LOADER)
    if (argc > 1)
    {
        printf("Loading %s\n", argv[1]);
    }
    else
    {
        printf("Payload parameter is empty\n");
        return -1;
    }

    send_elf(argv[1], false);
#else
    wait_close_disc_player = true;

    // Get the list of temp elf files created in PS5 JAR Loader
    vector<DirEntry> temp_elf_files1 = list_dir("/mnt/sandbox/download/NPXS40140/BD_BUDA/javatmp/queue");
    DirEntry::Sort(temp_elf_files1);
    vector<DirEntry> temp_elf_files2 = list_dir("/mnt/sandbox/download/NPXS40140/BD_BUDB/javatmp/queue");
    DirEntry::Sort(temp_elf_files2);
    vector<string> elf_files;

    // Copy to permanent location as temp files are deleted when Disc Player closted
    int index = 0;
    for (int i = 0; i < temp_elf_files1.size(); i++)
    {
        string elf_file = "/data/ps5-jar-loader/temp_payload" + to_string(index) + ".elf";
        notify("Payload %s queued", temp_elf_files1[i].name);
        copy(temp_elf_files1[i].path, elf_file);
        elf_files.push_back(elf_file);
        index++;
    }
    for (int i = 0; i < temp_elf_files2.size(); i++)
    {
        string elf_file = "/data/ps5-jar-loader/temp_payload" + to_string(index) + ".elf";
        notify("Payload %s queued", temp_elf_files2[i].name);
        copy(temp_elf_files2[i].path, elf_file);
        elf_files.push_back(elf_file);
        index++;
    }

    notify("Wait 12s then Close Disc Player\nfor queued payloads to load");
    start_time = get_tick();
    ret = 0;
    do
    {
        if (ret != 0)
            sleep(2);
        ret = find_process("SceDiscPlayer");
        delta_time = get_tick() - start_time;
    } while (ret == 0 && (delta_time < MAX_WAIT_TIME));
    sleep(3);

    if ( ret == 0)
    {
        notify("Timeout of 1min exceeded\nAborted loading queued payloads");
        goto cleanup;
    }

    for (int i=0; i < elf_files.size(); i++)
    {
        send_elf(elf_files[i], true);
        sleep(3);
    }

cleanup:
    for (int i=0; i < elf_files.size(); i++)
    {
        remove(elf_files[i].c_str());
    }
    rmdir("/data/ps5-jar-loader");
#endif

    return 0;
}
