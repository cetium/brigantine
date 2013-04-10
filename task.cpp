// Andrew Naplavkov

#include <chrono>
#include <exception>
#include "task.h"

task::task() : m_cancel(false)  {}
void task::set_frame(const frame& fr)  { m_fr = fr; }
void task::on_cancel()  { m_cancel = true; }

void task::run()
{
  using namespace std;
  try
  {
    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
    do_run();
    chrono::high_resolution_clock::time_point finish = chrono::high_resolution_clock::now();
    QString ms;
    ms.setNum(double(chrono::duration_cast<chrono::milliseconds>(finish - start).count()) / 1000., 'f', 1);
    emit signal_finished(QString("seconds: %1").arg(ms));
  }
  catch (const exception& e)
  {
    emit signal_finished(e.what());
  }
}
