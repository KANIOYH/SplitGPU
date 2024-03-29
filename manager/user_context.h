/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:19:02
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-22 19:07:08
 * @FilePath: /SplitGPU/manager/user_context.h
 * @Description:
 */
#pragma once
#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
namespace SplitGPU {

typedef int Client_id;

struct GPUMemory {
  std::size_t size;
  void *dptr;
};

class Ucontext {
public:
  Ucontext(Client_id client_id, int client_weight, size_t size);
  ~Ucontext();
  Client_id get_id();
  int get_weight();
  void set_weight(int weight);
  size_t get_memory_limit();
  size_t get_alloced_memory();
  void add_memory(size_t size, void *dptr);
  size_t remove_memory(void *dptr);
  void login_out(bool in);
  void log_launch();

private:
  Client_id _id;
  size_t _launch;
  int _weight;
  size_t _memory_limit;
  size_t _alloced_memory;
  std::shared_ptr<std::ofstream> _log;
  std::map<void *, GPUMemory> _memory_map;
};
} // namespace SplitGPU
