/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:19:02
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-02 12:05:35
 * @FilePath: /SplitGPU/include/user_context.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <cstddef>
#include <map>

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
    void remove_memory(void* dptr);
private:
    Client_id id;
    int    weight;
    size_t memory_limit;
    size_t alloced_memory;
    std::map<void*, GPUMemory> memory_map;
};
}

