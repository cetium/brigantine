// Andrew Naplavkov

#ifndef TASK_H
#define TASK_H

#include <chrono>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>
#include "frame.h"

class task : public QObject {
  Q_OBJECT

  enum Status {
    Waiting,
    Running,
    Canceled,
    Failed,
    Complete
  };

  QMutex m_mtx;
  const int m_id;
  const std::chrono::system_clock::time_point m_start;
  std::chrono::system_clock::time_point m_finish;
  Status m_st;
  QString m_msg;
  frame m_fr;

  bool is_finished_impl() const;

public slots:
  void on_cancel();
  void on_cancel(int id);

signals:
  void signal_finished(); // don't emit in inheritors

public:
  task();
  int get_id() const;
  std::chrono::system_clock::time_point get_finish();
  int get_milliseconds();
  QString get_status();
  bool is_finished();
  QString get_message();
  frame get_frame();
  void set_frame(const frame& fr);
  void run();

  virtual QString get_string() = 0;
  virtual int get_priority() = 0;
  virtual void progress(QString msg);
  virtual void run_impl() = 0;
}; // task

#endif // TASK_H
