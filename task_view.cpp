// Andrew Naplavkov

#include <QApplication>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
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

  m_cancel_act = new QAction("cancel this task", this);
  m_cancel_act->setIconVisibleInMenu(true);
  connect(m_cancel_act, SIGNAL(triggered()), this, SLOT(on_cancel()));

  m_cancel_all_act = new QAction("cancel all tasks", this);
  m_cancel_all_act->setIconVisibleInMenu(true);
  connect(m_cancel_all_act, SIGNAL(triggered()), this, SLOT(on_cancel_all()));

  m_clear_act = new QAction("clear", this);
  m_clear_act->setIconVisibleInMenu(true);
  connect(m_clear_act, SIGNAL(triggered()), this, SLOT(on_clear()));
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
  }
  actions.append(m_cancel_all_act);
  actions.append(m_clear_act);
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
  if (!isVisible()) m_mdl.vacuum(false);
}

void task_view::on_cancel()
{
  auto tsk = m_mdl.get_task(m_idx_menu);
  if (!tsk) return;
  QMessageBox dlg(QApplication::activeWindow());
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  dlg.setWindowIcon(QIcon(":/res/wheel.png"));
  dlg.setIcon(QMessageBox::Question);
  dlg.setText(QString("%1: %2").arg(tsk->get_id()).arg(tsk->get_string()));
  dlg.setInformativeText("Do you want to cancel the task?");
  dlg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  dlg.setDefaultButton(QMessageBox::Ok);
  if (dlg.exec() != QMessageBox::Ok) return;
  m_mdl.emit_cancel(int(m_idx_menu.internalPointer()));
}

void task_view::on_cancel_all()
{
  QMessageBox dlg(QApplication::activeWindow());
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  dlg.setWindowIcon(QIcon(":/res/wheel.png"));
  dlg.setIcon(QMessageBox::Question);
  dlg.setInformativeText("Do you want to cancel all tasks?");
  dlg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  dlg.setDefaultButton(QMessageBox::Ok);
  if (dlg.exec() != QMessageBox::Ok) return;
  m_mdl.emit_cancel_all();
}

void task_view::on_clear()
{
  m_mdl.vacuum(true);
}
