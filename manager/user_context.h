/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:19:02
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2023-12-30 16:18:56
 * @FilePath: /SplitGPU/manager/user_context.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <cstddef>
#include <map>
#include <vector>

namespace SplitGPU {

struct GPUMemory {
    std::size_t size;
    void* dptr;
};

class Ucontext {
public:
    Ucontext(size_t size);
    ~Ucontext();
    size_t get_memory_limit();
    size_t get_alloced_memory();
    void add_memory(size_t size, void* dptr);
    void remove_memory(void* dptr);
private:
    size_t memory_limit;
    size_t alloced_memory;
    std::map<void*, GPUMemory> memory_map;
};
}

