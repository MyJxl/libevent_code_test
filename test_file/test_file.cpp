#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <event2/event.h>

void read_file(evutil_socket_t fd, short event, void *arg)
{
    //printf("fd = %d\n", fd);
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf)-1);
    if(len > 0)
    {
        printf("buf = %s\n", buf);
    }
    else
    {
        printf("read_file. \n");
        sleep(1);
    }
    
}

int main(int argc, char **argv)
{
    event_config *conf = event_config_new();

    event_config_require_features(conf, EV_FEATURE_FDS);
    event_base *base = event_base_new_with_config(conf);
    event_config_free(conf);

    if(!base)
    {
        printf("event_base_new_with_config failed! \n");
        return -1;
    }

    int sock = open("/var/log/auth.log", O_RDONLY|O_NONBLOCK, 0);
    if(sock <= 0)
    {
        printf("open auth.log failed\n");
        return -2;
    }

    lseek(sock, 0, SEEK_END);

    event *fev = event_new(base, sock, EV_READ|EV_PERSIST, read_file, 0);
    event_add(fev, NULL);
    event_base_dispatch(base);
    event_base_free(base);

    return 0;
}