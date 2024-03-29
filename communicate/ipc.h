/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-30 16:59:18
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-06 22:25:18
 * @FilePath: /SplitGPU/include/ipc.h
 * @Description: 
 */
#pragma once
#include "split_gpu.h"
#include "user_context.h"
extern "C" {
    #include "shmem.h"
}
#include <mutex>

namespace SplitGPU {

enum Ipc_type {
    SHARE_MEMORY,
    TCP,
};

enum Request_state {
    REQUEST_FREE,
    REQUEST_READY,
    REQUEST_HANDLE,
    REQUEST_SUCC,
    REQUEST_FAIL,
    REQUEST_ERR,
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
    virtual ~Ipc_server() = default;
    virtual void start() = 0;
    virtual void close() = 0;
    virtual request* poll_requests() = 0;
    virtual void close_client(Client_id id) = 0;
};

class Ipc_client {
public:
    virtual ~Ipc_client() = default;
    virtual RET connect() = 0;
    virtual void close() = 0;
    virtual request* send_request(Request_type type,void* dptr,size_t size) = 0;
    virtual RET wait_request(request* req) = 0;
};

/*----------tcpip ipc----------*/
// not impl now
// class Tcp_server : public Ipc_server {
// public:
//     Tcp_server();
//     ~Tcp_server();
//     void start() override;
//     void close() override;
//     request* poll_requests() override;
// };

// class Tcp_client : public Ipc_client {
// public:
//     Tcp_client();
//     ~Tcp_client();
//     RET connect() override;
//     void close() override;
//     request* send_request(Request_type type,void* dptr,size_t size) override;
//     RET wait_request(request* req) override;
// };


/*----------shmem ipc----------*/
class Shm_server : public Ipc_server {
public:
    Shm_server();
    ~Shm_server() override;
    void start() override;
    void close() override;
    request* poll_requests() override;
    void close_client(Client_id id) override;
private:
    sg_shmem shm;
    int slot_number;
    int poll_idx;
    request* requests;
};

class Shm_client : public Ipc_client {
public:
    Shm_client();
    Shm_client(int user);
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