// Andrew Naplavkov

#ifndef TASK_H
#define TASK_H

#include <QMutex>
#include <QObject>
#include <QString>
#include "frame.h"

class task : public QObject {
  Q_OBJECT

  enum Status {
    Waiting,
    Running,
    Canceling,
    Canceled,
    Failed,
    Complete
  };

  QMutex m_mtx;
  const int m_id;
  const qint64 m_start;
  qint64 m_finish;
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
  qint64 get_finish();
  qint64 get_milliseconds();
  QString get_status();
  bool is_canceling();
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
