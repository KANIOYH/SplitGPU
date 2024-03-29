/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:46
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-02-06 21:02:08
 * @FilePath: /SplitGPU/include/controller.h
 * @Description: 
 * 
 */
#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "ipc.h"
#include "schedule.h"
#include "user_context.h"
#include "httplib.h"



namespace SplitGPU {

#define CTR_START (-22)

class Controller {
public:
    Controller(Ipc_type server_type, Schedule_type schedule_type);
    Controller(Ipc_type server_type, Schedule_type schedule_type,std::string intern_ip,int intern_port);
    void load_clients();
    void func_start();
    void intern_start();
private:
    RET register_client(Client_id id, int weight, size_t memory_size);
    RET deregister_client(Client_id id);
    RET client_online(Client_id id);
    RET client_offline(Client_id id);
private:    
    std::unique_ptr<Ipc_server> _ipc_server;
    std::thread _intern_server_thread;
    
    /* intern rpc */
    httplib::Server _intern_server;
    std::string _intern_ip;
    int _intern_port;

    /* watch dog */

    /* schedule handler */
    std::unique_ptr<Schedule> _schedule;
    size_t _memory;
    std::map<Client_id, Ucontext> ctxs;
};

}