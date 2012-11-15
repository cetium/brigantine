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

map_thread::map_thread(QObject* parent) : QThread(parent), m_abort(false), m_restart(false)  {}

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

void map_thread::render(std::vector<layer_link> lrs, const frame& fr)
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

void map_thread::render_layer(layer_link lr, const frame& fr, QImage& img, QString& msg, size_t& counter, QTime& time)
{
  if (!lr) return;
  try
  {
    QPainter painter(&img);
    painter.setCompositionMode(QPainter::CompositionMode_Darken);
    QImage lr_img(img.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter lr_painter(&lr_img);
    // todo: lr_painter.setRenderHint(QPainter::Antialiasing);
    lr_painter.eraseRect(lr_img.rect());
    lr_painter.setBrush(QColor(0, 0, 0, 24));
    const bool limited(lr.m_state == Qt::PartiallyChecked);
    const size_t counter_end(limited? counter + lr->limit(): LONG_MAX);

    std::vector<brig::database::variant> row;
    if (msg.isEmpty() && !lr->has_spatial_index(fr)) msg = "no index";
    for (auto rs(lr->drawing(fr, limited)); rs->fetch(row) && counter < counter_end; ++counter)
    {
      if (m_abort || (m_restart && !limited)) return;
      lr->draw(row, fr, lr_painter);
      if (time.elapsed() > SignalInterval)
      {
        if (!limited)
        {
          painter.drawImage(0, 0, lr_img);
          lr_painter.eraseRect(img.rect());
        }
        emit signal_process(fr, img);
        emit signal_process((msg.isEmpty()? to_string(counter + 1): msg));
        time.restart();
      }
    }

    if (counter >= counter_end) painter.setOpacity(painter.opacity() / 6.);
    painter.drawImage(0, 0, lr_img);
    emit signal_process((msg.isEmpty()? to_string(counter): msg));
  }
  catch (const std::exception& e)  { if (msg.isEmpty()) msg = QString::fromUtf8(e.what()); }
}

void map_thread::run()
{
  forever
  {
    m_mutex.lock();
    std::vector<layer_link> lrs = m_lrs;
    const frame fr(m_fr);
    m_mutex.unlock();

    std::sort(std::begin(lrs), std::end(lrs), [](const layer_link& a, const layer_link& b){ return a.m_order < b.m_order; });
    if (projPJ(fr.get_pj()) == 0) return;
    if (m_abort) return;
    emit signal_start();

    QString msg;
    size_t counter(0);
    QTime time; time.start();
    QImage img(fr.size(), QImage::Format_ARGB32_Premultiplied);
    {
      QPainter painter(&img);
      painter.eraseRect(img.rect());
    }

    for (int i(0); i < int(lrs.size()); ++i)
    {
      if (m_abort) return;
      if (m_restart) break;
      render_layer(lrs[i], fr, img, msg, counter, time);
    }
    if (m_abort) return;
    if (!m_restart)
    {
      emit signal_process(fr, img);
      emit signal_process(msg.isEmpty()? to_string(counter): msg);
      emit signal_idle();
    }

    m_mutex.lock();
    if (!m_restart) m_condition.wait(&m_mutex);
    m_restart = false;
    m_mutex.unlock();
  }
}
