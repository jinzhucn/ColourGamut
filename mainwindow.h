#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QButtonGroup>
#include "glcanvas.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void selectColorSpace (QAbstractButton*);

private:
  GLCanvas *_canvas;
  QButtonGroup *_buttons;

};

#endif // MAINWINDOW_H
