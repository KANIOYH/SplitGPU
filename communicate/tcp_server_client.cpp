/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-13 09:48:41
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-16 10:09:04
 * @FilePath: /SplitGPU/communicate/tcp_server_client.cpp
 * @Description: 
 * 
 */
#include <iostream>//控制台输出
#include <arpa/inet.h>
#include <netdb.h>
#include <semaphore.h>
#include <sys/socket.h>//创建socket
#include <sys/types.h>
#include <netinet/in.h>//socket addr
#include <sys/epoll.h>//epoll
#include <thread>
#include <unistd.h>//close函数
#include <fcntl.h>//设置非阻塞
#include <cstring>
#include "tcp_server_client.h"

using namespace std;


namespace SplitGPU {

Tcp_server::Tcp_server(int port):_port(port) {
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sock_addr.sin_port = htons(port);
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htons(INADDR_ANY);
	int flag = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
		printf("socket setsockopt error=%d(%s)!!!\n", errno, strerror(errno));
		exit(1);
	}

    if (bind(server_fd, (sockaddr *) &sock_addr, sizeof(sock_addr)) == -1) {
        cout << "bind error" << endl;
    }
    _write_events = std::list<tcp_event>();
    _read_events = std::list<tcp_event>();
    sem_init(&_read_event_sem,0,0);
}

Tcp_server::~Tcp_server() {
    poll_flag = false;
    poll_thread.join();
}

void Tcp_server::start_poll() {
    if (listen(server_fd, 10) == -1) {
        cout << "listen error" << endl;
        exit(-1);
    }   
    //创建一个epoll,size已经不起作用了,一般填1就好了
    int eFd = epoll_create(1);

    //把socket包装成一个epoll_event对象
    //并添加到epoll中
    epoll_event epev{};
    epev.events = EPOLLIN;//可以响应的事件,这里只响应可读就可以了
    epev.data.fd = server_fd;//socket的文件描述符
    epoll_ctl(eFd, EPOLL_CTL_ADD, server_fd, &epev);//添加到epoll中
    
    //回调事件的数组,当epoll中有响应事件时,通过这个数组返回
    epoll_event events[EVENTS_SIZE]; 
    int epoll_time = -1;
    poll_thread = std::thread([&]() {

    while (poll_flag) {
        while(!_write_event_mu.try_lock());
        for(auto it=_write_events.begin();it!=_write_events.end();) {
            write(it->client_fd, it->buff, MAX_DATA_SIZE);
            it = _write_events.erase(it);
        }
        _write_event_mu.unlock();
        //这个函数会阻塞,直到超时或者有响应事件发生
        if(_write_events.size() == 0) {
            epoll_time = 100;
        } else {
            epoll_time = 100;
        }
        int eNum = epoll_wait(eFd, events, EVENTS_SIZE, epoll_time);
        // if (eNum == -1) {
        //     cout << "epoll_wait" << endl;
        //     exit(-1);
        // }
        //遍历所有的事件
        for (int i = 0; i < eNum; i++) {
            //判断这次是不是socket可读(是不是有新的连接)
            if (events[i].data.fd == server_fd) {
                if (events[i].events & EPOLLIN) {
                    sockaddr_in cli_addr{};
                    socklen_t length = sizeof(cli_addr);
                    //接受来自socket连接
                    int fd = accept(server_fd, (sockaddr *) &cli_addr, &length);
                    if (fd > 0) {
                        //设置响应事件,设置可读和边缘(ET)模式
                        //很多人会把可写事件(EPOLLOUT)也注册了,后面会解释
                        epev.events = EPOLLIN | EPOLLET;
                        epev.data.fd = fd;
                        //设置连接为非阻塞模式
                        int flags = fcntl(fd, F_GETFL, 0);
                        if (flags < 0) {
                            cout << "set no block error, fd:" << fd << endl;
                            continue;
                        }
                        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
                            cout << "set no block error, fd:" << fd << endl;
                            continue;
                        }
                        //将新的连接添加到epoll中
                        epoll_ctl(eFd, EPOLL_CTL_ADD, fd, &epev);
                        cout << "client on line fd:" << fd << endl;
                    }
                }
            } else {//不是socket的响应事件
                
                //判断是不是断开和连接出错
                //因为连接断开和出错时,也会响应`EPOLLIN`事件
                if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
                    //出错时,从epoll中删除对应的连接
                    //第一个是要操作的epoll的描述符
                    //因为是删除,所有event参数null就可以了
                    epoll_ctl(eFd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    cout << "client out fd:" << events[i].data.fd << endl;
                    close(events[i].data.fd);
                } else if (events[i].events & EPOLLIN) {//如果是可读事件
                    tcp_event read_event;
                    //如果在windows中,读socket中的数据要用recv()函数
                    int len = read(events[i].data.fd, read_event.buff, MAX_DATA_SIZE);
                    //如果读取数据出错,关闭并从epoll中删除连接
                    if (len == -1) {
                        epoll_ctl(eFd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                        cout << "client out fd:" << events[i].data.fd << endl;
                        close(events[i].data.fd);
                    } else {
                        //读到的数据
                        read_event.client_fd = events[i].data.fd;
                        _read_events.push_back(read_event);
                        sem_post(&_read_event_sem);
                    }
                } //end else if
            } //end else
        } //end for
    } //end while

    }
    );

}

size_t Tcp_server::sock_write(int client_fd,char msg[]) {
    tcp_event event;
    event.client_fd = client_fd;
    memcpy(event.buff,msg,MAX_DATA_SIZE);
    while(!_write_event_mu.try_lock());
    _write_events.push_back(event);
    _write_event_mu.unlock();
    return 0;
}

tcp_event Tcp_server::read_event() {
    tcp_event event;
    event.client_fd = -1;
    int res = sem_wait(&_read_event_sem);
    //getchar();
    //while(!_write_event_mu.try_lock());
    event = _read_events.front();
    _read_events.erase(_read_events.begin());
    //_write_event_mu.unlock();
    return event;
}

Tcp_client::Tcp_client(std::string ipv4,std::string port):
_ipv4(ipv4),_port(port) {}

void Tcp_client::sock_connect() {
    addrinfo hints, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int gAddRes = getaddrinfo(_ipv4.c_str(), _port.c_str(), &hints, &p);
    if (gAddRes != 0)
    {
        std::cerr << gai_strerror(gAddRes) << "\n";
        exit(-1);
    }

    if (p == NULL)
    {
        std::cerr << "No addresses found\n";
        exit(-1);
    }

    _fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (_fd == -1)
    {
        std::cerr << "Error while creating socket\n";
        exit(-1);
    }

    int conn = connect(_fd, p->ai_addr, p->ai_addrlen);
    if (conn == -1)
    {
        close(_fd);
        std::cerr << "Error while connecting socket\n";
        exit(-1);
    }
}

size_t Tcp_client::sock_read(char msg[]) {
    return read(_fd, msg, MAX_DATA_SIZE);
}

size_t Tcp_client::sock_write(char msg[]) {
    return write(_fd, msg, MAX_DATA_SIZE);
}

size_t Tcp_client::sock_read(char msg[],size_t size) {
    return read(_fd, msg, size);
}

size_t Tcp_client::sock_write(char msg[],size_t size) {
    return write(_fd, msg, size);
}

}