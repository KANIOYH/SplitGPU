/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:04:13
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-06 22:42:20
 * @FilePath: /SplitGPU/manager/user_context.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "sg_log.h"
#include "split_gpu.h"
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include "user_context.h"
#include <chrono>
#include <ctime>

namespace SplitGPU {

    Ucontext::Ucontext(Client_id client_id,int client_weight, size_t size)
    :id(client_id), weight(client_weight), memory_limit(size),alloced_memory(0),launch(0) {
        log = std::make_shared<std::ofstream>("/home/chenyuanhui/project/SplitGPU/log/user/"+std::to_string(id)+".log");
        (*log) << "start" << std::endl;
    }

    Ucontext::~Ucontext(){
    }

    int Ucontext::get_weight() {
        return weight;
    }

    size_t Ucontext::get_memory_limit() {
        return memory_limit;
    }
    
    size_t Ucontext::get_alloced_memory() {
        return alloced_memory;
    }

    void Ucontext::add_memory(size_t size, void* dptr) {
        GPUMemory mem = {.size = size,.dptr = dptr,};
        memory_map[dptr] = mem;
        alloced_memory += size;
    }

    size_t Ucontext::remove_memory(void* dptr) {
        if(memory_map.find(dptr) != memory_map.end()) {
            GPUMemory mem = memory_map[dptr];
            alloced_memory -= mem.size;
            return mem.size;
        } else {
            SG_ERR("can not find memory");
            return -1;
        }
    }
    void Ucontext::log_launch() {
        launch++;
        if(launch%200==0) { 
        auto now = std::chrono::system_clock::now();  
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);  
        std::tm* local_time = std::localtime(&now_time);  
        std::string time(__TIME__);
            (*log) << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") 
            << " launched "<<std::to_string(launch) << std::endl;
        }
    };
    void Ucontext::login_out(bool in) {
        auto now = std::chrono::system_clock::now();  
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);  
        std::tm* local_time = std::localtime(&now_time);  
        std::string time(__TIME__);
        if(in) {
            (*log) << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") 
                        << " login" << std::endl;
        } else {
            (*log) << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") 
                        << " logout" << std::endl;
        }
    }

}