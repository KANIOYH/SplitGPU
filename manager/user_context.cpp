/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:04:13
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-22 19:08:54
 * @FilePath: /SplitGPU/manager/user_context.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "user_context.h"
#include "sg_log.h"
#include "split_gpu.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>

namespace SplitGPU {

Ucontext::Ucontext(Client_id client_id, int client_weight, size_t size)
    : _id(client_id), _weight(client_weight), _memory_limit(size),
      _alloced_memory(0), _launch(0) {
  _log = std::make_shared<std::ofstream>(
      "/home/chenyuanhui/project/SplitGPU/log/user/" + std::to_string(_id) +
      ".log");
  (*_log) << "start" << std::endl;
}

Ucontext::~Ucontext() {}

int Ucontext::get_weight() { return _weight; }

void Ucontext::set_weight(int weight) { _weight = weight; }

size_t Ucontext::get_memory_limit() { return _memory_limit; }

size_t Ucontext::get_alloced_memory() { return _alloced_memory; }

void Ucontext::add_memory(size_t size, void *dptr) {
  GPUMemory mem = {
      .size = size,
      .dptr = dptr,
  };
  _memory_map[dptr] = mem;
  _alloced_memory += size;
}

size_t Ucontext::remove_memory(void *dptr) {
  if (_memory_map.find(dptr) != _memory_map.end()) {
    GPUMemory mem = _memory_map[dptr];
    _alloced_memory -= mem.size;
    return mem.size;
  } else {
    SG_ERR("can not find memory");
    return -1;
  }
}
void Ucontext::log_launch() {
  _launch++;
  if (_launch % 200 == 0) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *local_time = std::localtime(&now_time);
    std::string time(__TIME__);
    (*_log) << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << " launched "
            << std::to_string(_launch) << std::endl;
  }
};
void Ucontext::login_out(bool in) {
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm *local_time = std::localtime(&now_time);
  std::string time(__TIME__);
  if (in) {
    (*_log) << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << " login"
            << std::endl;
  } else {
    (*_log) << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << " logout"
            << std::endl;
  }
}

} // namespace SplitGPU