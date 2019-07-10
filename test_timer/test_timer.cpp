#include <stdio.h>
#include <event2/event.h>

static timeval t1 = {1, 0};

void timer1(int sock, short which, void *arg)
{
    printf("[timer1 enter!] \n");

    // no pending
    event *ev = (event *)arg;
    if(!evtimer_pending(ev, &t1))
    {
        evtimer_del(ev);
        evtimer_add(ev, &t1);
    }
}

void timer2(int sock, short which, void *arg)
{
    printf("[timer2 enter!] \n");
}

void timer3(int sock, short which, void *arg)
{
    printf("[timer3 enter!] \n");
}

int main(int argc, char **argv)
{
    event_base *base = event_base_new();

    printf("test timer\n");
    event *evl = evtimer_new(base, timer1, event_self_cbarg());
    if(!evl)
    {
        printf("evetimer_new timer1 failed!\n");
    }

    evtimer_add(evl, &t1);

    static timeval t2;
    t2.tv_sec = 1;
    t2.tv_usec = 500000;
    event *ev2 = event_new(base, -1, EV_PERSIST, timer2, 0);
    event_add(ev2, &t2);

    // 超时性能优化, 默认event用二叉堆存储
    // 优化到双向队列
    event *ev3 = event_new(base, -1, EV_PERSIST, timer3, 0);
    static timeval tv_in = {3, 0};
    const timeval *t3;
    t3 = event_base_init_common_timeout(base, &tv_in);
    event_add(ev3, t3);

    event_base_dispatch(base);

    event_base_free(base);

    return 0;
}