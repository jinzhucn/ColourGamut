#include "mainwindow.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
  _statusBar = new QStatusBar(this);
  _statusBar->showMessage("Controls: 'X': rotate; 'Y': up and down; "
                          "'Z': depth");

  _canvas = new GLCanvas (this);

  setStatusBar(_statusBar);


  setCentralWidget(_canvas);
}

MainWindow::~MainWindow()
{
}
