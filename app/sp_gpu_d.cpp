/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:01:56
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-04-08 23:39:26
 * @FilePath: /SplitGPU/app/sp_gpu_d.cpp
 * @Description: GPU cuda-vir start here
 */

#include <thread>
#include "controller.h"
#include "httplib.h"
#include "ini.h"

int main() {
    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;
    file.read(ini);
    std::string& rpc_ipv4 = ini["rpc"]["ipv4"];
    std::string& rpc_port = ini["rpc"]["port"];
    std::string& watchdog_ipv4 = ini["watchdog"]["ipv4"];
    std::string& watchdog_port = ini["watchdog"]["port"];
    
    std::cout << "rpc_ipv4:" << rpc_ipv4 << std::endl;
    std::cout << "rpc_port:" << rpc_port << std::endl;
    std::cout << "watchdog_ipv4:" << watchdog_ipv4 << std::endl;
    std::cout << "watchdog_port:" << watchdog_port << std::endl;

    SplitGPU::Controller* ctlr = 
    new SplitGPU::Controller(
    SplitGPU::SHARE_MEMORY,
SplitGPU::TIME_SLICE_SCHEDULE,
"0.0.0.0",8999);
    ctlr->load_clients();
    ctlr->intern_start();
    ctlr->func_start();
    return 0;
}