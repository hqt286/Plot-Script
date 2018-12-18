#include <QApplication>
#include <QWidget>
#include <QProcess>
#include <QTextStream>
#include "notebook_app.hpp"
#include "QDebug"
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QWidget widget ;

  NotebookApp App;
  App.show();
  return app.exec();
}

