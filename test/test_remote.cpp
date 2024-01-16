/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-13 21:50:46
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-15 10:59:49
 * @FilePath: /SplitGPU/test/test_remote.cpp
 * @Description: 
 * 
 */

#include "tcp_server_client.h"
#include "gpu_node.h"
using namespace std;
using namespace SplitGPU;
int main() {
    Tcp_server ser(8888);
    ser.start_poll();
    char buf[64];
    GPU_node node;
    node.exec_cuda("cudaMalloc", buf);
    return 0;
}