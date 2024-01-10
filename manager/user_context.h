/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:19:02
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-04 11:46:10
 * @FilePath: /SplitGPU/include/user_context.h
 * @Description: 
 */
#pragma once
#include <cstddef>
#include <map>
#include <fstream>  
#include <iostream>  
#include <memory>
namespace SplitGPU {

typedef int Client_id;

struct GPUMemory {
    std::size_t size;
    void* dptr;
};

class Ucontext {
public:
    Ucontext(Client_id client_id, int client_weight, size_t size);
    ~Ucontext();
    Client_id get_id();
    int get_weight();
    size_t get_memory_limit();
    size_t get_alloced_memory();
    void add_memory(size_t size, void* dptr);
    size_t remove_memory(void* dptr);
    void login_out(bool in);
    void log_launch();
private:
    Client_id id;
    size_t launch;
    int    weight;
    size_t memory_limit;
    size_t alloced_memory;
    std::shared_ptr<std::ofstream> log;
    std::map<void*, GPUMemory> memory_map;
    
};
}

