#include "glcanvas.h"

#include <qmath.h>
#include <GL/glu.h>
#include <QKeyEvent>
#include <QDebug>

#include "color.h"

#define LAMBDA_MIN 380.01
#define LAMBDA_MAX 780.0

void glutWireCube( GLdouble dSize );

GLCanvas::GLCanvas(QWidget *parent)
  : QGLWidget(parent),  _delta_lambda (0.5),
    _Y (100), _delta_Y (100)
{
  _colorSpace = CIE_XYZ;

  _eyeX = 0;
  _eyeY = 0;


  _xCoord = 0.0;
  _yCoord = 0.5;
  _zCoord = 2.0;


  _N = ceil( (LAMBDA_MAX - LAMBDA_MIN) / _delta_lambda);

  _y_barSum = (float*) malloc (sizeof (float) * _N);
  if ( _y_barSum )
    for (int i = 0; i < _N; i++)
      _y_barSum[i] = -1.0f;

  int i = 0;
  for (float lambda = LAMBDA_MIN; lambda <= LAMBDA_MAX; lambda += _delta_lambda,
                                                                            i++)
  {
    float y_bar;
    float d65 = corGetD65(lambda);

    if (corGetCIExyz(lambda, 0, &y_bar, 0))
    {
      _K += y_bar * d65;

      if (i == 0)
        _y_barSum[i] = y_bar;
      else
        _y_barSum[i] = _y_barSum[i - 1]  + y_bar;
    }
  }

  _K = 100.0f / _K;

  QLinkedList <OptimalColor> optimalMountainColors;
  QLinkedList <OptimalColor> optimalValleyColors;

  computePoints(optimalMountainColors, MOUNTAIN);
  computePoints(optimalValleyColors, VALLEY);

  pointsCoordinatesFromIndexes(optimalMountainColors, _mountainPoints, MOUNTAIN);
  pointsCoordinatesFromIndexes(optimalValleyColors, _valleyPoints,
                               VALLEY);

  optimalMountainColors.clear();
  optimalValleyColors.clear();

  free (_y_barSum);

  setFocusPolicy(Qt::StrongFocus);
}

void GLCanvas::pointsCoordinatesFromIndexes(
    QLinkedList<OptimalColor>&optimalColors, QLinkedList<Point> &points,
    LAMBDA_TYPE type)
{
  foreach (const OptimalColor &color, optimalColors)
  {
    int spdSamples = _N;

    float *reflectanceSurfaceSPD = (float *)malloc (sizeof(float) * spdSamples);
    if (reflectanceSurfaceSPD)
    {
      for (int i = 0; i < spdSamples; i++)
      {
        if (color.index1 <= i  && i <= color.index2)
          reflectanceSurfaceSPD[i] = (type == MOUNTAIN) ? 1.0 : 0.0;
        else
          reflectanceSurfaceSPD[i] = (type == MOUNTAIN) ? 0.0 : 1.0;

      }

      float X;
      float Y;
      float Z;
      if (corCIEXYZfromSurfaceReflectance(LAMBDA_MIN, spdSamples, 1,
                                          reflectanceSurfaceSPD, &X, &Y,&Z,D65))
      {
        Point p;
        p.x = X;
        p.y = Y;
        p.z = Z;

        points.append(p);
      }
    }
  }
}

void GLCanvas::computePoints(QLinkedList<OptimalColor> &colors, LAMBDA_TYPE type)
{
  for (int i = 1; i <= _N; i++)
  {
    for (int j = 1; j <= _N; j++)
      if (i < j)
      {
        float Y;
        float y_bar_sum;

        if (type == MOUNTAIN)
          y_bar_sum = _y_barSum[j-1] - _y_barSum[i-1];
        else if(type == VALLEY)
          y_bar_sum = (_y_barSum[i-1] - _y_barSum[0]); +
                                    (_y_barSum[_N - 1] - _y_barSum[j - 1]);

        Y = _K * y_bar_sum;

        if (( _Y - _delta_Y <= Y)  &&
            ( Y <= _Y + _delta_Y))
        {
          OptimalColor color;
          color.index1 = i;
          color.index2 = j;

          colors.append(color);
        }
      }
  }
}

void GLCanvas::initializeGL()
{
  glClearColor(0.8, 0.8, 0.8, 0.0);
  glShadeModel (GL_FLAT);
  glEnableClientState (GL_VERTEX_ARRAY);
}

void GLCanvas::keyPressEvent(QKeyEvent *event)
{
  bool shift = event->modifiers() & Qt::ShiftModifier;

  int rotIncrement = 10;
  float depthIncrement = 0.1;

  switch (event->key())
  {
    case Qt::Key_X:
      if (shift)
        _eyeX += rotIncrement;
      else
        _eyeX -= rotIncrement;

      _eyeX = _eyeX % 360;

      update();
      break;

    case Qt::Key_Y:
      if (shift)
        _yCoord += depthIncrement;
      else
        _yCoord -= depthIncrement;

      _eyeY = _eyeY % 360;
      update();
      break;

    case Qt::Key_Z:
      if (shift)
        _zCoord += depthIncrement;
      else
        _zCoord -= depthIncrement;

      update();
      break;
  }

  qDebug () << _xCoord << "," << _yCoord << "," << _zCoord;

  QGLWidget::keyPressEvent(event);
}

void GLCanvas::resizeGL(int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(60.0, float(w)/float(h), 0.1f, 1000.f);

  gluLookAt(_xCoord, _yCoord, _zCoord, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  glTranslated(-10, -10, 0);
}

void GLCanvas::paintGL()
{  
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(_xCoord, _yCoord, _zCoord, 0.0, 0.0, 0.0, 0, 1, 0);

  glMatrixMode(GL_MODELVIEW);

  glRotatef(_eyeX, 0.0, 1.0, 0.0);

  glLineWidth(1);
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(1000.0, 0.0, 0.0);
  glEnd();

  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 1000, 0.0);
  glEnd();

  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 1000.0);
  glEnd();

  glColor3f(1.0, 1.0, 1.0);
  glPushMatrix();
  glTranslated(0.5, 0.5, 0.5);
  glutWireCube(1.0);
  glPopMatrix();


  glPointSize(1);

  glBegin(GL_POINTS);

  plotPoints(_mountainPoints);
  plotPoints(_valleyPoints);
  glEnd();

  glFlush ();
}

void GLCanvas::plotPoints(const QLinkedList<Point> &list)
{
  foreach (const Point &p, list)
  {
    float X, Y, Z;

    if (_colorSpace == CIE_RGB)
    {
      corCIEXYZtoCIERGB(p.x, p.y, p.z, &X, &Y, &Z);
    }
    else if (_colorSpace == CIE_LAB)
    {
      corCIEXYZtoLab(p.x, p.y, p.z, &Y, &X, &Z, D65);
      Y /= 100;
      X /= 100;
      Z /= 100;
    }
    else
    {
      X = p.x;
      Y = p.y;
      Z = p.z;
    }

    glColor3f (X, Y, Z);
    glVertex3f (X, Y, Z);
  }

}


void glutWireCube( GLdouble dSize )
{
    float size = (float) dSize * 0.5f;

#   define V(a,b,c) glVertex3f( a size, b size, c size );

    /*
     * PWO: I dared to convert the code to use macros...
     */
    glBegin( GL_LINE_LOOP ); V(-,-,+); V(+,-,+); V(+,+,+); V(-,+,+); glEnd();
    glBegin( GL_LINE_LOOP ); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-); glEnd();
    glBegin( GL_LINES );
        V(-,-,+); V(-,-,-); V(-,+,+); V(-,+,-);
        V(+,-,+); V(+,-,-); V(+,+,+); V(+,+,-);
    glEnd();

#   undef V
}
