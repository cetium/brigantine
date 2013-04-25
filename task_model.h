// Andrew Naplavkov

#ifndef TASK_MODEL_H
#define TASK_MODEL_H

#include <memory>
#include <QAbstractItemModel>
#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <vector>
#include "task.h"

class task_model : public QAbstractItemModel {
  Q_OBJECT
  std::vector<std::shared_ptr<task>> m_tsks;
  size_t m_activity;

protected:
  void timerEvent(QTimerEvent* event) override;

private slots:
  void on_finished();

signals:
  void signal_progress();
  void signal_idle();
  void signal_need_vacuum();
  void signal_cancel_all();
  void signal_cancel(int id);

public:
  explicit task_model(QObject* parent);
  ~task_model() override;

  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex&) const override  { return QModelIndex(); }
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  std::shared_ptr<task> get_task(const QModelIndex& index) const;
  void run(std::shared_ptr<task> tsk);
  void vacuum();
  void emit_cancel_all()  { emit signal_cancel_all(); }
  void emit_cancel(int id)  { emit signal_cancel(id); }
}; // task_model

#endif // TASK_MODEL_H
