/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-01 12:41:18
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-23 10:30:33
 * @FilePath: /SplitGPU/cs/controller.cpp
 * @Description: controller.cpp
 *
 */
#include <cstddef>
#include <future>
#include <memory>
#include <string>
#include <unistd.h>
#include <utility>

#include "controller.h"
#include "httplib.h"
#include "ipc.h"
#include "schedule.h"
#include "sg_log.h"
#include "split_gpu.h"
#include "user_context.h"

namespace SplitGPU {

Controller::Controller(Ipc_type server_type, Schedule_type schedule_type,
                       std::string intern_ip, int intern_port)
    : _memory(0), _intern_ip(intern_ip), _intern_port(intern_port) {
  switch (server_type) {
  default:
  case TCP:
  case SHARE_MEMORY: {
    _ipc_server = std::make_unique<Shm_server>();
  } break;
    // case TCP_SERVER: {
    // } break;
  }
  switch (schedule_type) {
  default:
  case TIME_SLICE_SCHEDULE: {

    _schedule = std::make_unique<Time_schedule>();
  } break;
  }
}

RET Controller::register_client(Client_id id, int weight, size_t memory_size) {
  if (ctxs.find(id) == ctxs.end()) {
    Ucontext ctx(id, weight, memory_size);
    ctxs.insert(std::pair<Client_id, Ucontext>(id, ctx));
    SG_LOG("register context, id:%d wegiht:%d memory:%ld", id, weight, memory_size/MB);
    return RET_OK;
  } else {
    SG_ERR("already have id");
    getchar();
    return RET_ERR;
  }
}

RET Controller::deregister_client(Client_id id) {
  auto item = ctxs.find(id);
  if (item != ctxs.end()) {
    ctxs.erase(item);
    _memory -= item->second.get_alloced_memory();
    SG_LOG("inline ctx:%ld,schedule:%d", ctxs.size(),
           _schedule->inline_number());
    return RET_OK;
  } else {
    SG_ERR("not find id");
    return RET_ERR;
  }
}

RET Controller::client_online(Client_id id) {
  auto item = ctxs.find(id);
  if (item != ctxs.end()) {
    _schedule->push_user(id, item->second.get_weight());
    return RET_OK;
  } else {
    SG_ERR("not find id,wait..");
    getchar();
    return RET_ERR;
  }
}

RET Controller::client_offline(Client_id id) {
  auto item = ctxs.find(id);
  if (item != ctxs.end()) {
    _schedule->remove_user(id, item->second.get_weight());
    return RET_OK;
  } else {
    SG_ERR("not find id,wait..");
    getchar();
    return RET_ERR;
  }
}

void Controller::intern_start() {

  /*init intern http-function*/

  _intern_server.Get("/exit",
                     [&](const httplib::Request &req, httplib::Response &res) {
                       std::string str_id = req.get_param_value("id");
                       int id = std::stoi(str_id);
                       auto item = ctxs.find(id);
                       if (item != ctxs.end()) {
                         item->second.login_out(false);
                         client_offline(id);
                       }
                       SG_LOG("exit get");
                       res.set_content("ok", "text");
                       res.status = httplib::OK_200;
                     });
                 
  _intern_server.Get("/register",
                     [&](const httplib::Request &req, httplib::Response &res) {
                       std::string str_id = req.get_param_value("id");
                       std::string str_weight = req.get_param_value("weight");
                       std::string str_memory = req.get_param_value("memory");
                       Client_id id = std::stoi(str_id);
                       int weight = std::stoi(str_weight);
                       int memory = std::stoi(str_memory);
                       RET ret = register_client(id, weight,memory * MB);
                       if(ret == RET_OK) {
                        res.set_content("ok", "text");
                        res.status = httplib::OK_200;
                       } else {
                        res.set_content("fail register client", "text");
                        res.status = httplib::OK_200;
                       }
                     });

  _intern_server.Get("/changeweight",
                     [&](const httplib::Request &req, httplib::Response &res) {
                       std::string str_id = req.get_param_value("id");
                       std::string str_weight = req.get_param_value("weight");
                       Client_id id = std::stoi(str_id);
                       int weight = std::stoi(str_weight);
                       auto item = ctxs.find(id);
                       int old_weight = 0;
                       RET ret;
                       if (item != ctxs.end()) {
                         old_weight = item->second.get_weight();
                         ret = _schedule->remove_user(id, item->second.get_weight());
                         if(ret != RET_OK) {
                            res.set_content("fail", "text");
                            res.status = httplib::OK_200;
                            return;
                         }
                         item->second.set_weight(weight);
                         ret = _schedule->push_user(id, item->second.get_weight());
                         if(ret != RET_OK) {
                            res.set_content("fail", "text");
                            res.status = httplib::OK_200;
                            return;
                         }
                            SG_LOG("set %d weight %d to %d", id,old_weight,weight);
                            res.set_content("ok", "text");
                            res.status = httplib::OK_200;
                       } else {
                            SG_LOG("not %d ", id);
                            res.set_content("fail", "text");                        
                       }

                     });

//   _intern_server.Get("userinfo", [&](const httplib::Request &req, httplib::Response &res) {
//                     res.set_content()
//   });

  _intern_server_thread = std::thread(
      [&]() { _intern_server.listen(_intern_ip.data(), _intern_port); });
  SG_LOG("start intern-rpc polling");
}

void Controller::func_start() {

  _schedule->start();
  _ipc_server->start();
  request *req = nullptr;
  RET ret;
  int sleep_level = 100;
  size_t times = 0;
  size_t hit = 0;
  SG_LOG("start polling..");
  while (true) {
    req = _ipc_server->poll_requests();
    if (req == nullptr) {
      if (sleep_level < 1000)
        sleep_level *= 2;
      // sleep(sleep_level);
      continue;
    }
    sleep_level = 100;
    switch (req->type) {
    case REQ_TYPE_INIT: {
      auto item = ctxs.find(req->user);
      if (item != ctxs.end()) {
        item->second.login_out(true);
      } else {
        req->state = REQUEST_FAIL;
      }
      SG_LOG("client:%d online", req->user);
      client_online(req->user);
      req->state = REQUEST_SUCC;
    } break;

    case REQ_TYPE_EXIT: {
      auto item = ctxs.find(req->user);
      if (item != ctxs.end()) {
        item->second.login_out(false);
      } else {
        req->state = REQUEST_FAIL;
      }
      client_offline(req->user);
      req->state = REQUEST_SUCC;
      SG_LOG("client:%d offline", req->user);
      _ipc_server->close_client(req->user);
    } break;

    case REQ_TYPE_ALLOC: {
      auto item = ctxs.find(req->user);
      if (item != ctxs.end()) {
        size_t try_alloc = req->size;
        if (req->dptr == nullptr) {
          size_t alloced = item->second.get_alloced_memory();
          size_t limit = item->second.get_memory_limit();
          if (try_alloc + alloced > limit) {
            SG_LOG("ID:%d try alloc limit,try:%ldMB,alloced:%ldMB,limit:%ldMB",
                   req->user, try_alloc / MB, alloced / MB, limit / MB);
            req->state = REQUEST_ERR;
          } else {
            req->state = REQUEST_SUCC;
          }
        } else {
          item->second.add_memory(try_alloc, req->dptr);
          _memory += try_alloc;
          SG_LOG("memory:%ldGB(%ldMB)", _memory / GB, _memory / MB);
          req->state = REQUEST_SUCC;
        }

      } else {
        SG_ERR("ID:%d not find ctx", req->user);
        req->state = REQUEST_ERR;
      }
    } break;

    case REQ_TYPE_FREE: {
      auto item = ctxs.find(req->user);
      if (item != ctxs.end()) {
        size_t size = item->second.remove_memory(req->dptr);
        if (size >= 0) {
          _memory -= size;
          req->state = REQUEST_SUCC;
          SG_LOG("memory:%ldGB(%ldMB)", _memory / GB, _memory / MB);
        } else {
          req->state = REQUEST_ERR;
        }
      } else {
        SG_ERR("ID:%d not find ctx", req->user);
        req->state = REQUEST_ERR;
      }
    } break;

    case REQ_TYPE_LAUNCH: {
      auto item = ctxs.find(req->user);
      if (item != ctxs.end()) {
        ret = _schedule->check(req->user, item->second.get_weight());
        if (ret == RET_OK) {
          req->state = REQUEST_SUCC;
          item->second.log_launch();
        } else {
          req->state = REQUEST_FAIL;
        }
      }

    } break;
    }
  }
}

void Controller::load_clients() {
  const int array_number = 1;
  int array_weight[array_number] = {20};
  size_t array_memory[array_number] = {1024 * MB};
  for (int i = 0; i < array_number; i++)
    register_client(i * i + i, array_weight[i], array_memory[i]);
}

} // namespace SplitGPU