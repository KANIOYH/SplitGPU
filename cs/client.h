/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:53
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-17 10:15:56
 * @FilePath: /SplitGPU/cs/client.h
 * @Description: 
 * 
 */
#pragma once
#include "ipc.h"
#include <memory>
#include <fstream>
#include<ctime>
#include <random>  
#include <cuda.h>

namespace SplitGPU {

class SplitClient {
public:
    SplitClient(Ipc_type ipc_type);
    SplitClient(Ipc_type ipc_type,bool remote);
    ~SplitClient();
    void connect();
    void close();
    RET request(Request_type type,void* dptr,size_t size);

private:
    std::string get_kernel_mangling_name(const void* host_fun);

private:
    std::shared_ptr<Ipc_client>  ipc_client;
    Ipc_type type;
    bool connected = false;
    bool remoted;
    std::map<const void*,std::string> func_name_table;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis; 
    
};
}