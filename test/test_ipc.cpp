/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-30 20:29:20
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-03 13:32:45
 * @FilePath: /SplitGPU/test/test_ipc.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "ipc.h"
#include "sg_log.h"
#include <cstdio>
#include <unistd.h>
namespace SplitGPU {

void server() {
    Ipc_server* server = new Shm_server();
    server->start();
    while(1) {
        request* req = server->poll_requests();
        if(req == nullptr) {
            SG_LOG("null");
            usleep(2000000);
        } else {
            req->state = REQUEST_SUCC;
        }
        
    }
    server->close();
}

void client() {
    Ipc_client* client = new Shm_client();
    SG_LOG("connect");
    client->connect();
    void* dptr;
    for(int i=0;i<200;i++) {
        request* req = client->send_request(REQ_TYPE_ALLOC,dptr,0);
        if (client->wait_request(req) != RET_OK) {
            SG_ERR("err in req:%d", i);
        } else {
            SG_LOG("%d OK i:%d",getpid(),i);
        }
        usleep(1000);
    }
}

}

int main(int argc, char **argv) {
    int opt;
    size_t size = 1;
    while ((opt = getopt(argc, argv, "cs::")) != -1)
    {
        switch (opt)
        {
        case 'c':
            SplitGPU::client();
            break;
        case 's':
            SplitGPU::server();
            break;
        default:
            break;
        }
    }

    return 0;
}
