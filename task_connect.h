// Andrew Naplavkov

#ifndef TASK_CONNECT_H
#define TASK_CONNECT_H

#include <memory>
#include <QString>
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
  QString m_str;
  std::shared_ptr<provider_allocator> m_allocator;

signals:
  void signal_connected(provider_ptr pvd, std::vector<layer_ptr> lrs);

public:
  explicit task_connect(std::shared_ptr<provider_allocator> allocator) : m_allocator(allocator), m_str("connect")  {}
  QString get_string() override  { return m_str; }
  int get_priority() override  { return 2; }
  void do_run(QEventLoop& loop) override;
}; // task_connect

#endif // TASK_CONNECT_H
