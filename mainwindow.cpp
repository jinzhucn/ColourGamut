#include "mainwindow.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
  QHBoxLayout *layout = new QHBoxLayout;

  _canvas = new GLCanvas (this);
  layout->addWidget(_canvas);

  setCentralWidget(_canvas);
}

MainWindow::~MainWindow()
{
}
