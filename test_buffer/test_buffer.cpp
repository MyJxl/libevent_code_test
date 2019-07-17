#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

void event_cb(bufferevent *be, short events, void *arg)
{
    printf("\033[41m[%s:%s:%d] events = %x \033[0m\n", __FILE__, __FUNCTION__, __LINE__, events);
    
    //
    if(events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
    {
        printf("BEV_EVENT_TIMEOUT BEV_EVENT_READING\n");
        //bufferevent_enable(be, EV_READ | EV_WRITE);
        bufferevent_free(be);
    }
    else if(events & BEV_EVENT_ERROR)
    {
        bufferevent_free(be);
    }
    else
    {
        printf("other event\n");
    }
}


void write_cb(bufferevent *be, void *arg)
{
    printf("\033[41m[%s:%s:%d] \033[0m\n", __FILE__, __FUNCTION__, __LINE__);
}

void read_cb(bufferevent *be, void *arg)
{
    printf("\033[41m[%s:%s:%d] \033[0m\n", __FILE__, __FUNCTION__, __LINE__);

    char data[1024] = {0};
	//读取输入缓冲数据
	int len = bufferevent_read(be,data,sizeof(data)-1);
	 printf("\033[41m[%s:%s:%d] data = %s \033[0m\n", __FILE__, __FUNCTION__, __LINE__, data);
	if(len<=0)
        return;
	if(strstr(data,"stop") !=NULL)
	{
		printf("stop\n");
		//退出并关闭socket BEV_OPT_CLOSE_ON_FREE
		bufferevent_free(be);
	}
	//发送数据 写入到输出缓冲
	bufferevent_write(be,"OK",3);

}   

void listen_cb(evconnlistener *ev, evutil_socket_t s, sockaddr *sin, int slen, void *arg)
{
    printf("\033[41m[%s:%s:%d] \033[0m\n", __FILE__, __FUNCTION__, __LINE__);

    event_base *base = (event_base *)arg;
    
    // 创建bufferevent 上下文, BEV_OPT_CLOSE_ON_FREE 清理bufferevent时关闭socket
    bufferevent * bev= bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
    
    // 添加监控事件
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    // 设置水位

    // 读取水位
    bufferevent_setwatermark(bev, 
        EV_READ,
        5, // 低水位, 0 是无限制
        10); // 高水位

    // 超时时间设置
    timeval t1 = {3, 0};
    bufferevent_set_timeouts(bev, &t1, 0);

    // 设置回调函数
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
}

int main(int argc, char **argv)
{
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        return -1;
    }

    event_base *base = event_base_new();
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(5001);

    evconnlistener *ev = evconnlistener_new_bind(base,
        listen_cb,  // 回调函数
        base,       // 回调函数的参数arg
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        10,         // listen backlog
        (sockaddr *)&sin,
        sizeof(sin));
   
    // 进入事件主循环
    event_base_dispatch(base);
    evconnlistener_free(ev);
    event_base_free(base);

    return 0;
}