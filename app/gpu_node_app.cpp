/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-15 14:23:59
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-16 16:29:12
 * @FilePath: /SplitGPU/app/gpu_node_app.cpp
 * @Description: 
 * 
 */
#include "gpu_node.h"
#include "tcp_server_client.h"

using namespace std;
using namespace SplitGPU;

int main() {
    GPU_node g_node;
    Tcp_server t_ser(8888);
    t_ser.start_poll();
    tcp_event event;
    while(true) {
        event = t_ser.read_event();
        if(event.client_fd < 0)
            continue;
        gpu_request* req = (gpu_request*)event.buff;
        g_node.exec_cuda(req->func_name,req->fargs);
        t_ser.sock_write(event.client_fd, event.buff);
    }
    
}