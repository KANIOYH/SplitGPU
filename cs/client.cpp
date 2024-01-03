/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:25
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-02 17:02:05
 * @FilePath: /SplitGPU/cs/client.cpp
 * @Description: 
 * 
 */
#include "client.h"
#include "ipc.h"
#include "sg_log.h"
#include "split_gpu.h"
#include <memory>

namespace SplitGPU {

Client::Client(Ipc_type ipc_type):type(ipc_type) {}

void Client::connect() {
    if(connected) {
        return;
    }
    switch (type) {
    case SHARE_MEMORY:
    case TCP: {
        ipc_client = std::make_shared<Shm_client>();
    } break;
    }
    ipc_client->connect();
    connected = true;
    request(REQ_TYPE_INIT,nullptr,0);
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
    RET ret = ipc_client->wait_request(req);
    while(ret == RET_FAIL) {
        req = ipc_client->send_request(type, dptr, size);
        ret = ipc_client->wait_request(req);
    }
    return ret;
}

}