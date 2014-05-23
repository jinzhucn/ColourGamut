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
    _Y (0.5), _delta_Y (0.5)
{
  _eyeX = 2;
  _eyeY = 2;
  _eyeZ = 5;

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

  computeType1Points();

  foreach (const OptimalColor &color, _optimalColors)
  {
    int spdSamples = _N;

    float *reflectanceSurfaceSPD = (float *)malloc (sizeof(float) * spdSamples);
    if (reflectanceSurfaceSPD)
    {
      for (int i = 0; i < spdSamples; i++)
        if (color.index1 <= i  && i <= color.index2)
          reflectanceSurfaceSPD[i] = 1.0;
        else
          reflectanceSurfaceSPD[i] = 0.0;

      float X;
      float Y;
      float Z;
      if (corCIEXYZfromSurfaceReflectance(LAMBDA_MIN, spdSamples, 1,
                                          reflectanceSurfaceSPD, &X, &Y,&Z,D65))
      {
        Point p;
        p.x = X * 2;
        p.y = Y * 2;
        p.z = Z * 2;

        _pointMountains.append(p);
      }
    }
  }

  _optimalColors.clear();

//  computeType2Points();
  foreach (const OptimalColor &color, _optimalColors)
  {
    float *reflectanceSurfaceSPD = (float *) malloc (sizeof(float) * _N);
    if (reflectanceSurfaceSPD)
    {
      for (int i = 0; i < _N; i++)
        if (color.index1 <= i  && i <= color.index2)
          reflectanceSurfaceSPD[i] = 0.0;
        else
          reflectanceSurfaceSPD[i] = 1.0;

      float X;
      float Y;
      float Z;
      if (corCIEXYZfromSurfaceReflectance(LAMBDA_MIN, _N, _delta_lambda,
                                          reflectanceSurfaceSPD, &X, &Y,&Z,D65))
      {
        Point p;
        p.x = X * 2;
        p.y = Y * 2;
        p.z = Z * 2;

        _pointValleys.append(p);
      }
    }
  }


  setFocusPolicy(Qt::StrongFocus);

}

void GLCanvas::computeType1Points()
{
//  float lambda = LAMBDA_MIN;
  for (int i = 1; i <= _N; i++)
  {
    for (int j = 1; j <= _N; j++)
      if (i < j)
      {
        float Y;
        float y_bar_sum = _y_barSum[j-1] - _y_barSum[i-1];

        Y = _K * y_bar_sum;

        if (( _Y - _delta_Y <= Y)  &&
            ( Y <= _Y + _delta_Y))
        {
          OptimalColor color;
          color.index1 = i;
          color.index2 = j;

          _optimalColors.append(color);
        }
      }
  }
}

void GLCanvas::computeType2Points()
{
  float lambda = LAMBDA_MIN;
  for (int i = 1; i <= _N; i++)
    for (int j = 1; j <= _N; j++)
      if (i < j)
      {
        float Y;
        float y_bar_sum = 0;
        for (int k = 1; k <= i; k++)
        {
          float y_bar;
          if (corGetCIExyz(lambda + k * _delta_lambda, 0, &y_bar, 0))
            y_bar_sum += y_bar;
        }

        for (int k = j; k <= _N; k++)
        {
          float y_bar;
          if (corGetCIExyz(lambda + k * _delta_lambda, 0, &y_bar, 0))
            y_bar_sum += y_bar;
        }

        Y = _K * y_bar_sum;

        if (( _Y - _delta_Y <= Y)  &&
            ( Y <= _Y + _delta_Y))
        {
          OptimalColor color;
          color.index1 = i;
          color.index2 = j;

          _optimalColors.append(color);
        }
      }
}

void GLCanvas::initializeGL()
{
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glShadeModel (GL_FLAT);
}

void GLCanvas::keyPressEvent(QKeyEvent *event)
{
  bool shift = event->modifiers() & Qt::ShiftModifier;

  float increment = 0.5;

  switch (event->key())
  {
    case Qt::Key_X:
      if (shift)
        _eyeX += increment;
      else
        _eyeX -= increment;

      update();
      break;

    case Qt::Key_Y:
      if (shift)
        _eyeY += increment;
      else
        _eyeY -= increment;

      update();
      break;

    case Qt::Key_P:
      if (shift)
        _eyeZ += increment;
      else
        _eyeZ -= increment;

      update();
      break;
  }

  qDebug () << _eyeX << "," << _eyeY << "," << _eyeZ;

  QGLWidget::keyPressEvent(event);
}

void GLCanvas::resizeGL(int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(50.0, float(w)/float(h), 0.1f, 1000.f);

  gluLookAt(_eyeX, _eyeY, _eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void GLCanvas::paintGL()
{  
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(_eyeX, _eyeY, _eyeZ, 0.0, 0.0, 0.0, 0, 1, 0);

  glColor3f(0.0, 0.0, 0.0);


  glMatrixMode(GL_MODELVIEW);

  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(1000.0, 0.0, 0.0);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 1000, 0.0);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 1000.0);
  glEnd();

  glPushMatrix();
  glTranslated(0.5, 0.5, 0.5);
  glutWireCube(1.0);
  glPopMatrix();


  glPointSize(1);
  glBegin(GL_POINTS);

  foreach (const Point &p, _pointMountains)
  {
    glColor3f (p.x, p.y, p.z);
    glVertex3f (p.x, p.y, p.z);
  }

  foreach (const Point &p, _pointValleys)
  {
    glColor3f (p.x, p.y, p.z);
    glVertex3f (p.x, p.y, p.z);
  }


  glEnd();

  glFlush ();
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
