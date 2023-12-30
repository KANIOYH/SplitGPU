/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:03:17
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2023-12-30 19:31:48
 * @FilePath: /SplitGPU/communicate/rpc.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "ipc.h"
#include <unistd.h>

namespace SplitGPU {

Shm_server::Shm_server() {
    poll_idx = 0;
    sg_init_shmem(&shm); 
    int req_byte = sizeof(request);
    int req_number = (SHM_SIZE - req_byte) / req_byte;
    int* share_number_ptr = reinterpret_cast<int*>(shm.shmaddr);
    requests = reinterpret_cast<request*>(shm.shmaddr + req_byte);
    *share_number_ptr = req_number;
    slot_number = req_number;
    SG_LOG("init Shm_server,request number:%d", req_number);
    for(int i=0;i<slot_number;i++) {
        requests[i].state = REQ_FREE;
    }
}

Shm_server::~Shm_server() {
    sg_free_shmem(&shm);
}

void Shm_server::start() {}

void Shm_server::close() {}

request* Shm_server::poll_requests() {
    int idx = 0;
    for(int i=0;i<slot_number;i++) {
        idx = (i + poll_idx) % slot_number;
        if(requests[idx].state == REQ_READY) {
            poll_idx = idx;
            return &(requests[idx]);
        }
    }
    return nullptr;
}


Shm_client::Shm_client():user(getpid()) {}

Shm_client::~Shm_client() {
    sg_free_shmem(&shm);
}

RET Shm_client::connect() {
    sg_init_shmem(&shm); 
    int req_byte = sizeof(request);
    int req_number = (SHM_SIZE - req_byte) / req_byte;
    requests = reinterpret_cast<request*>(shm.shmaddr + req_byte);
    int* share_number_ptr = reinterpret_cast<int*>(shm.shmaddr);
    slot_number = *share_number_ptr;
    return RET_OK;
}

void Shm_client::close() {}

request* Shm_client::send_request(Request_type type,void* dptr,size_t size){
    int idx = 0;
    for(int i=0;i<slot_number;i++) {
        if(requests[i].mutex.try_lock()) {
            requests[i].user  = user;
            requests[i].dptr  = dptr;
            requests[i].size  = size;
            requests[i].type  = type;
            requests[i].state = REQ_READY;
            return &requests[i];
        }
    }    
    return nullptr;
}

RET Shm_client::wait_request(request* req) {
    while (!(req->state == REQ_SUCC || req->state == REQ_FAIL)) {
        continue;
    }
    req->mutex.unlock();
    if(req->state == REQ_SUCC)
        return RET_OK;
    else if(req->state == REQ_FAIL)
        return RET_FAIL;
    else
        return RET_ERR;
}

}
