// Andrew Naplavkov

#include <algorithm>
#include <climits>
#include <exception>
#include <QMutexLocker>
#include <QPainter>
#include <Qt>
#include "layer.h"
#include "map_thread.h"
#include "utilities.h"

map_thread::map_thread(QObject* parent) : QThread(parent)
{
  // todo: MSVC November 2012 CTP - atomic_bool constructor problem
  m_abort = false;
  m_restart = false;
}

map_thread::~map_thread()
{
  m_mutex.lock();
  m_abort = true;
  m_condition.wakeOne();
  m_mutex.unlock();

  if (!wait(TimeOut))
  {
    terminate();
    wait();
  }
}

void map_thread::render(std::vector<layer_ptr> lrs, const frame& fr)
{
  QMutexLocker locker(&m_mutex);

  m_lrs = lrs;
  m_fr = fr;

  if (!isRunning())
    start(LowPriority);
  else
  {
    m_restart = true;
    m_condition.wakeOne();
  }
}

void map_thread::render_layer(layer_ptr lr, const frame& fr, QImage& img, QString& msg, bool& no_idx, size_t& counter, QTime& time)
{
  if (!lr) return;
  try
  {
    if (!lr->has_spatial_index(fr)) no_idx = true;

    QPainter painter(&img);
    painter.setCompositionMode(QPainter::CompositionMode_Darken);
    QImage lr_img(img.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter lr_painter(&lr_img);
    // todo: lr_painter.setRenderHint(QPainter::Antialiasing);
    lr_painter.eraseRect(lr_img.rect());
    lr_painter.setBrush(QColor(0, 0, 0, 24));

    std::vector<brig::variant> row;
    for (auto rs(lr->drawing(fr)); rs->fetch(row); ++counter)
    {
      if (m_abort.load() || m_restart.load()) return;
      lr->draw(row, fr, lr_painter);
      if (time.elapsed() > SignalInterval)
      {
        painter.drawImage(0, 0, lr_img);
        lr_painter.eraseRect(img.rect());
        emit signal_process(fr, img);
        emit signal_process((msg.isEmpty()? QString("%1").arg(counter + 1): msg));
        time.restart();
      }
    }

    painter.drawImage(0, 0, lr_img);
    emit signal_process((msg.isEmpty()? QString("%1").arg(counter): msg));
  }
  catch (const std::exception& e)  { if (msg.isEmpty()) msg = QString::fromUtf8(e.what()); }
}

void map_thread::run()
{
  forever
  {
    m_mutex.lock();
    std::vector<layer_ptr> lrs = m_lrs;
    const frame fr(m_fr);
    m_mutex.unlock();

    std::sort(std::begin(lrs), std::end(lrs), [](const layer_ptr& a, const layer_ptr& b){ return a.m_order < b.m_order; });
    if (projPJ(fr.get_pj()) == 0) return;
    if (m_abort.load()) return;
    emit signal_start();

    QString msg;
    size_t counter(0);
    QTime time; time.start();
    QImage img(fr.size(), QImage::Format_ARGB32_Premultiplied);
    {
      QPainter painter(&img);
      painter.eraseRect(img.rect());
    }

    bool no_idx(false);
    for (int i(0); i < int(lrs.size()); ++i)
    {
      if (m_abort.load()) return;
      if (m_restart.load()) break;
      render_layer(lrs[i], fr, img, msg, no_idx, counter, time);
    }
    if (m_abort.load()) return;
    if (!m_restart.load())
    {
      if (msg.isEmpty() && no_idx) msg = "no index";
      emit signal_process(fr, img);
      emit signal_process(msg.isEmpty()? QString("%1").arg(counter): msg);
      emit signal_idle();
    }

    m_mutex.lock();
    if (!m_restart.load()) m_condition.wait(&m_mutex);
    m_restart = false;
    m_mutex.unlock();
  }
}
