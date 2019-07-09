#include <stdio.h>
#include <signal.h>
#include <event2/event.h>

static void Ctrl_c(int sock, short which, void *arg)
{
    printf("RECV SIGINT\n");
}

static void Kill(int sock, short which, void *arg)
{
    printf("RECV SIGKILL\n");
    
    event *ev = (event *)arg;

    // 如果处于非待决状态, 
    if(!evsignal_pending(ev, NULL))
    {
        event_del(ev);
        event_add(ev, NULL);
    }
}

int main(int argc, char **argv)
{
    event_base *base = event_base_new();

    // 添加 ctrl+c 信号
    // evsignal_new 隐藏的状态  EV_SIGNAL | EV_PERSIST
    event *csig = evsignal_new(base, SIGINT, Ctrl_c, base);
    if(!csig)
    {
        printf("SIGINT evsignal_new failed!");
        return -1;
    }

    // 添加事件到pending
    if(event_add(csig, 0) != 0)
    {
        printf("SIGINT event_add failed\n");
        return -1;
    }

    // 添加kill信号, 只进入一次
    // event_self_cbarg 传递当前的event
    event *ksig = event_new(base, SIGTERM, EV_SIGNAL, Kill, event_self_cbarg());
    if(!ksig)
    {
        printf("SIGTERM evsignal_new failed!\n");
        return -1;
    }

    if(event_add(ksig, 0) != 0)
    {
        printf("SIGTERM event_add failed!\n");
        return -1;
    }

    // 进入事件主循环
    event_base_dispatch(base);
    event_free(csig);
    event_base_free(base);

    return 0;
}