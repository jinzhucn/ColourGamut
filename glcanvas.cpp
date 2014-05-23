#include "glcanvas.h"
#include "color.h"
#include <qmath.h>

#include <GL/glu.h>

#define LAMBDA_MIN 380.1

GLCanvas::GLCanvas(QWidget *parent)
  : QGLWidget(parent),  _delta_lambda (0.1),
    _Y (0.2), _delta_Y (0.01)
{
  _N = ceil( 400.0/_delta_lambda);

  for (float lambda = LAMBDA_MIN; lambda <= 780.0; lambda += 5.0)
  {
    float y_bar;
    float d65 = corGetD65(lambda);

    if (corGetCIExyz(lambda, 0, &y_bar, 0))
      _K += y_bar * d65;
  }

  _K = 100.0 / _K;

  computeType1Points();

  foreach (OptimalColor color, _optimalColors)
  {
    float *reflectanceSurfaceSPD = (float *) malloc (sizeof(float) * _N);
    if (reflectanceSurfaceSPD)
    {
      for (int i = 0; i < _N; i++)
        if (color.index1 <= i  && i <= color.index2)
          reflectanceSurfaceSPD[i] = 1.0;
        else
          reflectanceSurfaceSPD[i] = 0.0;

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

        _points.append(p);
      }
    }
  }

//  for (float lambda = float(400); lambda < float(700); lambda+= 0.01)
//  {
//    float cieX;
//    float cieY;
//    float cieZ;

//    float X;
//    float Y;
//    float Z = 0;

//    if (corGetCIExyz(lambda, &cieX, &cieY, &cieZ) &&
//        corCIEXYZtoxyY(cieX, cieY, cieZ, &X, &Y))
//    {
//      Point p;

//      corCIEXYZtosRGB(X, Y, Z, &p.r, &p.g, &p.b, D65);

//      p.x = X;
//      p.y = Y;
//      p.z = Z;
//      _points.append(p);
//    }
//  }
}


int GLCanvas::getYBar(float lambda, float *y_bar)
{
  if (!y_bar) return 0;

  *y_bar = _y_barValuesFromLambda.value(lambda, -1);

  if (*y_bar == -1)
  {
    if (corGetCIExyz(lambda, 0, y_bar, 0))
    {
      _y_barValuesFromLambda.insert(lambda, *y_bar);
      return 1;
    }
    else
      return 0;
  }

  return 1;
}

void GLCanvas::computeType1Points()
{
  float lambda = LAMBDA_MIN;
  for (int i = 1; i <= _N; i++)
    for (int j = 1; j <= _N; j++)
      if (i < j)
      {
        float Y;
        float y_bar_sum = 0;
        for (int k = i; k <= j; k++)
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

void GLCanvas::resizeGL(int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity ();
  glOrtho(-3.5, 3.5, -3.5, 3.5, -20.0, 20.0);
  gluLookAt(0.2, 0.2, 0.5, 0.0, 0.0, 0.0, 0, 1, 0);
  glMatrixMode(GL_MODELVIEW);
//  glTranslated(1, 1, 0);
//  glScaled(s, s, s);
}

void GLCanvas::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0.0, 0.0, 0.0);

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

  glPointSize(4);
  glBegin(GL_POINTS);

  foreach (const Point &p, _points)
  {
    glVertex3f (p.x, p.y, p.z);
  }


  glEnd();

  glFlush ();
}
