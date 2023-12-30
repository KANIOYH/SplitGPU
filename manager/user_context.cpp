/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:04:13
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2023-12-30 17:28:30
 * @FilePath: /SplitGPU/manager/user_context.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "split_gpu.h"
#include "user_context.h"

namespace SplitGPU {

    Ucontext::Ucontext(size_t size):memory_limit(size) {}

    Ucontext::~Ucontext(){}

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

    void Ucontext::remove_memory(void* dptr) {
        if(memory_map.find(dptr) != memory_map.end()) {
            GPUMemory mem = memory_map[dptr];
            alloced_memory += mem.size;
        } else {
            SG_ERR("can not find memory");
        }
    }

}