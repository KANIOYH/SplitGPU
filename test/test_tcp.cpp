#include <unistd.h>
#include "tcp_server_client.h"
using namespace std;
using namespace SplitGPU;

void client_func() {
    Tcp_client cli("127.0.0.1","8888");
    cli.sock_connect();
    char buf[]="hello~";
    cli.sock_write(buf);
    char rbuf[MAX_DATA_SIZE]={0};
    cli.sock_read(rbuf);
    //getchar();
    printf("%s\n",rbuf);
}

void server_func() {
    Tcp_server ser(8888);
    ser.start_poll();
    tcp_event event = ser.read_event();
    printf("read event:%d %s\n",event.client_fd,event.buff);
    char buf[]="reply!";
    int res = ser.sock_write(event.client_fd, buf);
    getchar();
}

int main(int argc, char **argv) {
    int opt;
    int process_type = 0;
    int ipc_type = 0;
    while ((opt = getopt(argc, argv, "sc:::-")) != -1) {
        switch (opt)
        {
        case 's':
        default:
            printf("server\n");
            server_func();
            break;
        case 'c':
        printf("client\n");
            client_func();
            break;
        }
    }   

    return 0;
}