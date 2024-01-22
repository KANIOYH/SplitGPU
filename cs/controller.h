/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:46
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-06 22:16:50
 * @FilePath: /SplitGPU/cs/controller.h
 * @Description: 
 * 
 */
#pragma once

#include <cstddef>
#include <map>
#include <thread>

#include "ipc.h"
#include "schedule.h"
#include "user_context.h"
#include "httplib.h"



namespace SplitGPU {

// struct ClientInfo {
//     Client_id id;
//     int schedule_weight;
//     int memory_limit;
// };

// class ClientInforTable {
// public:
//     std::shared_ptr<ClientInfo> query_client();
//     void update_client()
// };

class Controller {
public:
    Controller(Ipc_type server_type, Schedule_type schedule_type);
    void load_clients();
    void func_start();
    void intern_start();
private:
    RET register_client(Client_id id, int weight, size_t memory_size);
    RET deregister_client(Client_id id);
    RET client_online(Client_id id);
    RET client_offline(Client_id id);
private:    
    Ipc_server* server;
    std::thread intern_server_thread;
    httplib::Server intern_server;
    Schedule* schedule;
    size_t memory;
    std::map<Client_id, Ucontext> ctxs;
};

}