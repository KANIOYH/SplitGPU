/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:53
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-02 11:29:02
 * @FilePath: /SplitGPU/include/client.h
 * @Description: 
 * 
 */
#pragma once
#include "ipc.h"
#include <memory>

namespace SplitGPU {


class Client {
public:
    Client(Ipc_type ipc_type);
    ~Client();
    void connect();
    RET request(Request_type type,void* dptr,size_t size);
private:
    std::shared_ptr<Ipc_client>  ipc_client;
    Ipc_type type;
    bool connected = false;
    
};
}