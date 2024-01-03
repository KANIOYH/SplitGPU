/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:18
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-02 22:58:45
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
#include <unistd.h>
#include <utility>

namespace SplitGPU {

Controller::Controller(Ipc_type server_type, Schedule_type schedule_type) {
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
        schedule->push_user(id, weight);
        SG_LOG("register context, id:%d",id);
        return RET_OK;
    } else {
        SG_ERR("already have id");
        getchar();
        return RET_ERR;
    }
}

RET Controller::deregister_client(Client_id id) {
    if(ctxs.find(id) != ctxs.end()) {
        schedule->remove_user(id, ctxs.find(id)->second.get_weight());
        ctxs.erase((ctxs.find(id)));
        SG_LOG("inline ctx:%ld,schedule:%d",ctxs.size(),schedule->inline_number());
        return RET_OK;
    } else {
        SG_ERR("not find id");
        return RET_ERR;
    }    
}

void Controller::start() {
    schedule->start();
    server->start();
    request* req = nullptr;
    RET ret;
    SG_LOG("start polling..");
    while(true) {
        req = server->poll_requests();
        if(req == nullptr) {
            usleep(10000);
            continue;
        }
        // printf("\n");
        // SG_LOG("get req, user:%d,type:%d", req->user, req->type);
        switch (req->type) {
        case REQ_TYPE_INIT: {
            register_client(req->user,10,1024*MB);
            req->state = REQ_SUCC;
        } break;
        case REQ_TYPE_ALLOC: {

        } break;
        case REQ_TYPE_FREE: {

        } break;
        case REQ_TYPE_LAUNCH: {
            auto item = ctxs.find(req->user);
            if(item != ctxs.end()) {
                ret = schedule->check(req->user, item->second.get_weight());
                if(ret == RET_OK) {
                    req->state = REQ_SUCC;
                } else {
                    req->state = REQ_FAIL;
                }
            }
            
        } break;
        case REQ_TYPE_EXIT: {
            deregister_client(req->user);
            req->state = REQ_SUCC;
        } break;
        }
    }
    
}

}