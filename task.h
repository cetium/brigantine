// Andrew Naplavkov

#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QEventLoop>
#include <QString>
#include "frame.h"

class task : public QObject {
  Q_OBJECT
protected:
  frame m_fr;
  bool m_cancel;
public slots:
  void on_cancel();
signals:
  void signal_progress(QString msg);
  void signal_finished(QString msg); // don't emit in inheritors
public:
  task();
  void set_frame(const frame& fr);
  void run();
  virtual QString get_string() = 0;
  virtual int get_priority() = 0;
  virtual void do_run(QEventLoop& loop) = 0;
}; // task

#endif // TASK_H
