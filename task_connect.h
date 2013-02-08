// Andrew Naplavkov

#ifndef TASK_CONNECT_H
#define TASK_CONNECT_H

#include <memory>
#include <vector>
#include "layer_ptr.h"
#include "provider_ptr.h"
#include "task.h"

class task_connect : public task {
  Q_OBJECT

public:
  struct provider_allocator {
    virtual ~provider_allocator()  {}
    virtual provider_ptr allocate() = 0;
  }; // provider_allocator

private:
  std::vector<std::shared_ptr<provider_allocator>> m_allocators;

signals:
  void signal_connected(provider_ptr pvd, std::vector<layer_ptr> lrs);

public:
  explicit task_connect(const std::vector<std::shared_ptr<provider_allocator>>& allocators) : m_allocators(allocators)  {}
  void run(progress* prg) override;
}; // task_connect

#endif // TASK_CONNECT_H
