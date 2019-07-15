#include <stdio.h>
#include <event2/event.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#define SPORT 5001

void client_cb(evutil_socket_t s, short w, void *arg)
{
    event *ev = (event *)arg;

    if(w&EV_TIMEOUT)
    {
        printf("timeout\n");
        event_free(ev);
        evutil_closesocket(s);
        return;         
    }
    char buf[1024] = {0};
    int len = recv(s, buf, sizeof(buf)-1, 0);
    if(len > 0)
    {
        printf("buf = %s\n", buf);      
        send(s, "ok", 2, 0);
    }
    else
    {
        printf("event_free\n");
        event_free(ev);
        evutil_closesocket(s);
    }
}

void listen_cb(evutil_socket_t s, short w, void *arg)
{
    printf("listen_cb\n");
    sockaddr_in sin;
    socklen_t size = sizeof(sin);
    evutil_socket_t client= accept(s, (sockaddr *)&sin, &size);
    char ip[17] = {0};
    evutil_inet_ntop(AF_INET, &sin.sin_addr, ip, sizeof(ip)-1);
    printf("clinet ip is %s\n", ip);

    // 客户端数据读取事件
    event_base *base = (event_base *)arg;
    event *ev = event_new(base, client, EV_READ|EV_PERSIST, client_cb, event_self_cbarg());
    timeval t = {10, 0};
    event_add(ev, &t);
}

int main(int argc, char **argv)
{
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        return -1;
    }

    event_base *base = event_base_new();
    printf("test event sever\n");

    evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock <= 0)
    {
        printf("socket error, errno = %d", errno);
        return  -1;
    }

    // 设置地址复用和非阻塞
    evutil_make_socket_nonblocking(sock);
    evutil_make_listen_socket_reuseable(sock);

    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);
    int re = bind(sock, (sockaddr *)&sin, sizeof(sin));
    if(re != 0)
    {
        printf("bind error: %d\n", errno);
        return -1;
    }

    listen(sock, 10);
    // 默认水平触发
    event *ev = event_new(base, sock, EV_READ|EV_PERSIST, listen_cb, base);
    event_add(ev, 0);

    event_base_dispatch(base);
    evutil_closesocket(sock);
    event_base_free(base);

    return 0;
}