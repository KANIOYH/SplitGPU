/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:25
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-23 10:17:07
 * @FilePath: /SplitGPU/cs/client.cpp
 * @Description: 
 * 
 */
#include "client.h"
#include "ipc.h"
#include "sg_log.h"
#include "split_gpu.h"
#include <cstdio>
#include <memory>
#include <string>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>



namespace SplitGPU {



Client::Client(Ipc_type ipc_type):type(ipc_type) {
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<>(0, 99); 
}

// Client::Client(Ipc_type ipc_type, bool remote)
//     :type(ipc_type),remoted(remote) {
//     gen = std::mt19937(rd());
//     dis = std::uniform_int_distribution<>(0, 99); 
// }

void Client::connect() {
    if(connected) {
        return;
    }
    int key = 0;
    printf("watch in\n");
    /*be watched*/
    {
        int sockfd = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in cli;//创建结构体
        bzero(&cli,sizeof(cli));
        cli.sin_family = AF_INET;
        cli.sin_addr.s_addr = inet_addr("127.0.0.1");
        cli.sin_port = htons(8889);
        ::connect(sockfd,(sockaddr*)&cli,sizeof(cli));
        int pid = getpid();
        printf("pid:%d\n",pid);
        std::string str_pid = std::to_string(pid);
        printf("pid:%s\n",str_pid.data());
        ssize_t size = write(sockfd,str_pid.data(),str_pid.size());
        if(size == -1 ){
        }
        char buf[16];
        //char need[]="A";
        ssize_t read_size = read(sockfd,buf,sizeof(buf));
        key = std::stoi(buf);
        if(key == -1) {
            printf("key to gpu check fail,key:%d\n",key);
            ::close(sockfd);
            exit(0);
        }
        if(size > 0 ){
        }
        else if(size == 0){
        }
        else if(size == -1){
        ::close(sockfd);
        }
        ::close(sockfd);
    }
    printf("watch out\n");
    switch (type) {
    case SHARE_MEMORY:
    case TCP: {
        ipc_client = std::make_shared<Shm_client>(key);
    } break;
    }
    ipc_client->connect();
    connected = true;
    request(REQ_TYPE_INIT,nullptr,0);
}

void Client::close() {
    if(connected) {
        RET ret = request(REQ_TYPE_EXIT,nullptr,0);
        connected = false;
        exit(-1);
    }
}

Client::~Client() {
    if(connected) {
        request(REQ_TYPE_EXIT,nullptr,0);
    }
}

RET Client::request(Request_type type,void* dptr,size_t size) {
    if(!connected) {
        connect();
    }
    auto req = ipc_client->send_request(type, dptr, size);
    int times = 0;
    RET ret = ipc_client->wait_request(req);
    while(ret == RET_FAIL) {
        if(times<2)
            times++;
        else 
            usleep(TIME_SLICE_UNIT*dis(gen));
        req = ipc_client->send_request(type, dptr, size);
        ret = ipc_client->wait_request(req);

    }
    return ret;
}

}