// Andrew Naplavkov

#include <QHeaderView>
#include <QMenu>
#include "task_view.h"

task_view::task_view(QWidget* parent) : QTableView(parent), m_mdl(0)
{
  setModel(&m_mdl);
  setContextMenuPolicy(Qt::CustomContextMenu);
  verticalHeader()->hide();

  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_show_menu(QPoint)));
  connect(&m_mdl, SIGNAL(signal_progress()), this, SLOT(emit_progress()));
  connect(&m_mdl, SIGNAL(signal_idle()), this, SLOT(emit_idle()));
  connect(&m_mdl, SIGNAL(signal_need_vacuum()), this, SLOT(on_need_vacuum()));

  m_cancel_all_act = new QAction("cancel all tasks", this);
  m_cancel_all_act->setIconVisibleInMenu(true);
  connect(m_cancel_all_act, SIGNAL(triggered()), this, SLOT(on_cancel_all()));

  m_cancel_act = new QAction("cancel this task", this);
  m_cancel_act->setIconVisibleInMenu(true);
  connect(m_cancel_act, SIGNAL(triggered()), this, SLOT(on_cancel()));

  m_separator_act = new QAction("", this);
  m_separator_act->setSeparator(true);
}

void task_view::on_finished()
{
  if (qobject_cast<task*>(sender())->get_id() == int(m_idx_menu.internalPointer()))
    m_cancel_act->setEnabled(false);
}

void task_view::on_show_menu(QPoint point)
{
  m_idx_menu = indexAt(point);
  auto tsk = m_mdl.get_task(m_idx_menu);

  QList<QAction*> actions;
  if (tsk)
  {
    m_cancel_act->setEnabled(!tsk->is_finished());
    actions.append(m_cancel_act);
    actions.append(m_separator_act);
  }
  actions.append(m_cancel_all_act);
  QMenu::exec(actions, mapToGlobal(point));
}

void task_view::on_task(std::shared_ptr<task> tsk)
{
  connect(tsk.get(), SIGNAL(signal_finished()), this, SLOT(on_finished()));
  m_mdl.run(tsk);
}

void task_view::emit_progress()
{
  m_cancel_all_act->setEnabled(true);
  emit signal_progress();
}

void task_view::emit_idle()
{
  m_cancel_all_act->setEnabled(false);
  emit signal_idle();
}

void task_view::on_need_vacuum()
{
  if (!isVisible()) m_mdl.vacuum();
}

void task_view::on_cancel_all()
{
  m_mdl.emit_cancel_all();
}

void task_view::on_cancel()
{
  m_mdl.emit_cancel(int(m_idx_menu.internalPointer()));
}
