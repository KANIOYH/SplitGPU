/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:46
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-01 22:59:10
 * @FilePath: /SplitGPU/cs/controller.h
 * @Description: 
 * 
 */
#pragma once
#include "ipc.h"
#include "schedule.h"
#include "user_context.h"
#include <cstddef>
#include <map>

namespace SplitGPU {


class Controller {
public:
    Controller(Ipc_type server_type, Schedule_type schedule_type);
    void start();
private:
    RET register_client(Client_id id, int weight, size_t memory_size);
    RET deregister_client(Client_id id);
private:    
    Ipc_server* server;
    Schedule* schedule;
    std::map<Client_id, Ucontext> ctxs;
};

}