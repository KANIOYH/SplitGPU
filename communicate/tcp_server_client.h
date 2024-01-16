/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-13 10:16:30
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-16 16:27:59
 * @FilePath: /SplitGPU/communicate/tcp_server_client.h
 * @Description: 
 * 
 */
#include <list>
#include <mutex>
#include <semaphore.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

namespace SplitGPU {

#define MAX_DATA_SIZE (512)

struct tcp_event {
    int client_fd;
    char buff[MAX_DATA_SIZE];
};

class Tcp_server {
public:
    Tcp_server(int port);
    ~Tcp_server();
    void start_poll();
    size_t sock_write(int client_fd,char msg[]);
    tcp_event read_event();
private:
    const int EVENTS_SIZE = 20;
    int server_fd;
    sockaddr_in sock_addr;
    int _port;
    std::thread poll_thread;
    bool poll_flag = true;;
    std::list<tcp_event> _write_events;
    std::mutex _write_event_mu;
    std::list<tcp_event> _read_events;
    sem_t _read_event_sem;
    std::mutex _read_event_mu;
};
 
class Tcp_client {
public:
    Tcp_client(std::string ipv4,std::string port);
    ~Tcp_client() {}
    void sock_connect();
    size_t sock_read(char msg[]);
    size_t sock_write(char msg[]);
    size_t sock_read(char msg[],size_t size);
    size_t sock_write(char msg[],size_t size);
private:
    std::string _ipv4;
    std::string _port;
    int _fd;
    char buff[MAX_DATA_SIZE];
};

}