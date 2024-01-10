/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:25
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-10 13:31:22
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
#include <unistd.h>



namespace SplitGPU {



Client::Client(Ipc_type ipc_type):type(ipc_type) {
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<>(0, 99); 
}

Client::Client(Ipc_type ipc_type, bool remote)
    :type(ipc_type),remoted(remote) {
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<>(0, 99); 
}

void Client::connect() {
    if(connected) {
        return;
    }
    switch (type) {
    case SHARE_MEMORY:
    case TCP: {
        ipc_client = std::make_shared<Shm_client>(0);
    } break;
    }
    ipc_client->connect();
    connected = true;
    request(REQ_TYPE_INIT,nullptr,remoted);
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