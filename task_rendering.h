// Andrew Naplavkov

#ifndef TASK_RENDERING_H
#define TASK_RENDERING_H

#include <QImage>
#include <QString>
#include "frame.h"
#include "layer_ptr.h"
#include "task.h"

class task_rendering : public task {
  Q_OBJECT
  layer_ptr m_lr;
  void emit_progress(QString wrn, size_t counter);
signals:
  void signal_image(frame fr, QImage img);
public:
  explicit task_rendering(layer_ptr lr) : m_lr(lr)  {}
  QString get_string() override;
  int get_priority() override  { return 0; }
  void do_run(QEventLoop& loop) override;
}; // task_rendering

#endif // TASK_RENDERING_H
