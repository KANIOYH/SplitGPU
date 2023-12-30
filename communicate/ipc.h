/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-30 16:59:18
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2023-12-30 19:54:14
 * @FilePath: /SplitGPU/include/ipc.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "split_gpu.h"
//#include "tcp.h"
extern "C" {
    #include "shmem.h"
}

#include <mutex>
namespace SplitGPU {

enum Request_state {
    REQ_FREE,
    REQ_READY,
    REQ_HANDLE,
    REQ_SUCC,
    REQ_FAIL,
};

enum Request_type {
    REQ_TYPE_INIT,
    REQ_TYPE_ALLOC,
    REQ_TYPE_FREE,
    REQ_TYPE_LAUNCH,
    REQ_TYPE_EXIT,
};

struct request {
    int user;
    std::mutex mutex;
    Request_state state;
    Request_type type;
    void* dptr;
    size_t size;
};

class Ipc_server {
public:
    Ipc_server();
    virtual void start();
    virtual void close();
    virtual request* poll_requests();
};

class Ipc_client {
public:
    virtual RET connect();
    virtual void close();
    virtual request* send_request(Request_type type,void* dptr,size_t size);
    virtual RET wait_request(request* req);
};

/*----------shmem ipc----------*/
class Shm_server : public Ipc_server {
public:
    Shm_server();
    ~Shm_server();
    void start() override;
    void close() override;
    request* poll_requests() override;
private:
    sg_shmem shm;
    int slot_number;
    int poll_idx;
    request* requests;
};

class Shm_client : public Ipc_client {
public:
    Shm_client();
    ~Shm_client();
    RET connect() override;
    void close() override;
    request* send_request(Request_type type,void* dptr,size_t size) override;
    RET wait_request(request* req) override;

private:
    int user;
    sg_shmem shm;
    int slot_number;
    request* requests;
};

}