/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:18
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-22 16:18:54
 * @FilePath: /SplitGPU/cs/controller.cpp
 * @Description: controller.cpp
 * 
 */
#include "controller.h"
#include "ipc.h"
#include "schedule.h"
#include "sg_log.h"
#include "split_gpu.h"
#include "user_context.h"
#include <cstddef>
#include <future>
#include <unistd.h>
#include <utility>

namespace SplitGPU {

Controller::Controller(Ipc_type server_type, Schedule_type schedule_type):memory(0) {
    switch (server_type) {
        default:
        case TCP:
        case SHARE_MEMORY: {
            schedule = new Time_schedule();
        } break;
        // case TCP_SERVER: {
        // } break;
    }
    switch (schedule_type) {
        default:
        case TIME_SLICE_SCHEDULE: {
            server = new Shm_server();
        } break;
    }
}

RET Controller::register_client(Client_id id, int weight, size_t memory_size) {
    if(ctxs.find(id) == ctxs.end()) {
        Ucontext ctx(id,weight,memory_size);
        ctxs.insert(std::pair<Client_id, Ucontext>(id,ctx));
        SG_LOG("register context, id:%d",id);
        return RET_OK;
    } else {
        SG_ERR("already have id");
        getchar();
        return RET_ERR;
    }
}

RET Controller::deregister_client(Client_id id) {
    auto item = ctxs.find(id);
    if(item != ctxs.end()) {
        ctxs.erase(item);
        memory -= item->second.get_alloced_memory();
        SG_LOG("inline ctx:%ld,schedule:%d",ctxs.size(),schedule->inline_number());
        return RET_OK;
    } else {
        SG_ERR("not find id");
        return RET_ERR;
    }    
}

RET Controller::client_online(Client_id id) {
    auto item = ctxs.find(id);
    if(item != ctxs.end()) {
        schedule->push_user(id, item->second.get_weight());
        return RET_OK;
    } else {
        SG_ERR("not find id,wait..");
        getchar();
        return RET_ERR;
    }
}

RET Controller::client_offline(Client_id id) {
    auto item = ctxs.find(id);
    if(item != ctxs.end()) {
        schedule->remove_user(id, item->second.get_weight());
        return RET_OK;
    } else {
        SG_ERR("not find id,wait..");
        getchar();
        return RET_ERR;
    }
}

void Controller::start() {
    schedule->start();
    server->start();
    request* req = nullptr;
    RET ret;
    int sleep_level = 100;
    size_t times=0;
    size_t hit=0;
    SG_LOG("start polling..");
    while(true) {
        req = server->poll_requests();
        if(req == nullptr) {
            if(sleep_level < 1000)
                sleep_level*=2;
            sleep(sleep_level);
            continue;
        }
        sleep_level = 100;
        switch (req->type) {
        case REQ_TYPE_INIT: {
            auto item = ctxs.find(req->user);
            if(item != ctxs.end()) {
                    item->second.login_out(true);
            } else {
                req->state = REQUEST_FAIL;
            }
            SG_LOG("client:%d online",req->user);
            client_online(req->user);
            req->state = REQUEST_SUCC;
        } break;

        case REQ_TYPE_EXIT: {
            auto item = ctxs.find(req->user);
            if(item != ctxs.end()) {
                    item->second.login_out(false);
            } else {
                req->state = REQUEST_FAIL;
            }
            client_offline(req->user);
            req->state = REQUEST_SUCC;
            SG_LOG("client:%d offline",req->user);
            server->close_client(req->user);
        } break;

        case REQ_TYPE_ALLOC: {
            auto item = ctxs.find(req->user);
            if(item != ctxs.end()) {
                size_t try_alloc = req->size;
                if(req->dptr == nullptr) {
                    size_t alloced = item->second.get_alloced_memory();
                    size_t limit = item->second.get_memory_limit();
                    if(try_alloc + alloced > limit) {
                        SG_LOG("ID:%d try alloc limit,try:%ldMB,alloced:%ldMB,limit:%ldMB",
                        req->user,try_alloc/MB,alloced/MB,limit/MB);
                        req->state = REQUEST_ERR;
                    } else {
                        req->state = REQUEST_SUCC;
                    }
                } else {
                    item->second.add_memory(try_alloc, req->dptr);
                    memory += try_alloc;
                    SG_LOG("memory:%ldGB(%ldMB)", memory/GB,memory/MB);
                    req->state = REQUEST_SUCC;
                }

            } else {
                SG_ERR("ID:%d not find ctx",req->user);
                req->state = REQUEST_ERR;
            }
        } break;

        case REQ_TYPE_FREE: {
            auto item = ctxs.find(req->user);
            if(item != ctxs.end()) {
                size_t size = item->second.remove_memory(req->dptr);
                if(size >= 0) {
                    memory -= size;
                    req->state = REQUEST_SUCC;
                    SG_LOG("memory:%ldGB(%ldMB)", memory/GB,memory/MB);
                } else {
                    req->state = REQUEST_ERR;
                }
            } else {
                SG_ERR("ID:%d not find ctx",req->user);
                req->state = REQUEST_ERR;
            }
        } break;

        case REQ_TYPE_LAUNCH: {
            auto item = ctxs.find(req->user);
            if(item != ctxs.end()) {
                ret = schedule->check(req->user, item->second.get_weight());
                if(ret == RET_OK) {
                    req->state = REQUEST_SUCC;
                    item->second.log_launch();
                } else {
                    req->state = REQUEST_FAIL;
                }
            }
            
        } break;
        
        }
    }
    
}

void Controller::load_clients() {
    const int array_number = 3;
    int array_weight[array_number] = {20,30,50};
    size_t array_memory[array_number] = {1024*MB,512*MB,512*MB};
    for(int i=0;i<array_number;i++)
        register_client(i*i+i, array_weight[i], array_memory[i]);
}

}