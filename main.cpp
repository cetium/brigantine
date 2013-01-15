// Andrew Naplavkov

#include <QApplication>
#include "main_window.h"

class MyApplication : public QApplication {
public:
  MyApplication(int & argc, char ** argv)
    : QApplication(argc, argv)
  {}

  bool notify(QObject * receiver, QEvent * event) override
  {
    try  { return QApplication::notify(receiver, event); }
    catch (const std::exception&)  {}
    return false;
  }
};

int main(int argc, char *argv[])
{
    MyApplication a(argc, argv);
    main_window w;
    w.show();
    return a.exec();
}
