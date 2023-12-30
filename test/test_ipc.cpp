#include "ipc.h"
namespace SplitGPU {

void server() {
    Ipc_server server = Shm_server();
    server.start();
    for(int i=0;i<201;i++) {
        request* req = server.poll_requests();
        req->state = REQ_SUCC;
    }
    server.close();
}

void client() {
    Ipc_client client = Shm_client();
    client.connect();
    void* dptr;
    for(int i=0;i<200;i++) {
        request* req = client.send_request(REQ_TYPE_ALLOC,dptr,0);
        if (client.wait_request(req) != RET_OK) {
            SG_ERR("err in req:%d", i);
        }
    }
}

int main(int argc, char **argv) {
    int opt;
    size_t size = 1;
    while ((opt = getopt(argc, argv, "cs:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            client();
            break;
        case 's':
            server();
            break;
        default:
            break;
        }
    }

    return 0;
}

}
