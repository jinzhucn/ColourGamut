#include "mainwindow.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
  _canvas = new GLCanvas (this);

  _buttons = new QButtonGroup();
  _buttons->setExclusive(true);

  QHBoxLayout *buttonsLayout = new QHBoxLayout;

  QRadioButton *cieXYZSpace = new QRadioButton("CIE XYZ");
  cieXYZSpace->setChecked(true);
  QRadioButton *cieRGBSpace = new QRadioButton("CIE RGB");

  _buttons->addButton(cieXYZSpace);
  _buttons->addButton(cieRGBSpace);

  buttonsLayout->addWidget(cieXYZSpace);
  buttonsLayout->addWidget(cieRGBSpace);

  QGroupBox *groupBox = new QGroupBox("Color Space", this);
  groupBox->setLayout(buttonsLayout);

  groupBox->resize(200, 50);


  setCentralWidget(_canvas);
  statusBar()->showMessage("Controls: 'X': rotate; 'Y': up and down; "
                          "'Z': depth");

  connect (_buttons, SIGNAL(buttonClicked(QAbstractButton*)),
           this, SLOT(selectColorSpace (QAbstractButton*)));
}

void MainWindow::selectColorSpace(QAbstractButton *button)
{
  if (button->text() == "CIE XYZ")
    _canvas->selectColorSpace(CIE_XYZ);
  else if (button->text() == "CIE RGB")
    _canvas->selectColorSpace(CIE_RGB);
}

MainWindow::~MainWindow()
{
}
